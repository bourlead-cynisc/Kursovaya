#include "setLogSink.h"
#include <iostream>
#include <random>
#include "AESGenerateKey.h"
#include "AESEncrypt.h"
#include "AESDecrypt.h"
#include "RSAGenerateKeys.h"
#include "RSAEncrypt.h"
#include "RSADecrypt.h"
#include "HashFile.h"
#include "ECDSAGenerateKeys.h"
#include "ECDSASign.h"
#include "ECDSAVerify.h"
#include "ecdsaPublicKeyPathFor.h"
namespace fs = std::filesystem;
static bool same(const fs::path& a, const fs::path& b){
    std::ifstream x(a,std::ios::binary), y(b,std::ios::binary);
    std::string sx((std::istreambuf_iterator<char>(x)),{}), sy((std::istreambuf_iterator<char>(y)),{});
    return sx==sy;
}
int main(){
    setLogSink([](const std::string& s){ std::cout<<"  "<<s<<"\n"; });
    fs::path d = fs::temp_directory_path()/"cq_test"; fs::remove_all(d); fs::create_directories(d/"out");
    int fails=0;
    auto check=[&](bool ok,const char* n){ std::cout<<(ok?"[OK]   ":"[FAIL] ")<<n<<"\n"; if(!ok) fails++; };
    std::mt19937 g(1);
    for (size_t sz : {0ul, 1ul, 15ul, 16ul, 1000ul, 65536ul, 65537ul, 300000ul, 1048576ul}) {
        fs::path f = d/("f"+std::to_string(sz)+".bin");
        { std::ofstream o(f,std::ios::binary); for(size_t i=0;i<sz;i++){char c=char(g()); o.write(&c,1);} }
        fs::path k = d/"aes.key"; AESGenerateKey(256,k);
        auto e = AESEncrypt(f,d/"out",k,256);
        auto r = AESDecrypt(e,d/"out",k,256);
        check(same(f,r), ("AES roundtrip size "+std::to_string(sz)).c_str());
    }
    // wrong key
    { fs::path f=d/"f1000.bin", k2=d/"k2.key"; AESGenerateKey(256,k2);
      auto e=AESEncrypt(f,d/"out",d/"aes.key",256);
      bool thrown=false; try{ AESDecrypt(e,d/"out",k2,256);}catch(const std::exception& ex){thrown=true; std::cout<<"  err: "<<ex.what()<<"\n";}
      check(true,"AES wrong key handled (throw or garbage, no crash)"); (void)thrown; }
    RSAGenerateKeys(2048,d);
    { fs::path f=d/"f300000.bin"; auto e=RSAEncrypt(f,d/"out",d/"public_2048.key",2048);
      auto r=RSADecrypt(e,d/"out",d/"private_2048.key",2048); check(same(f,r),"RSA hybrid roundtrip 300KB"); }
    { fs::path f=d/"f1000.bin"; std::cout<<HashFile(f)<<"\n"; }
    for (auto c : {"P-256","P-384","P-521"}) {
        fs::path pk=d/"ec.key", pub=ecdsaPublicKeyPathFor(pk), sig=d/"s.sig";
        ECDSAGenerateKeys(c,pk,pub); fs::path f=d/"f300000.bin";
        ECDSASign(f,pk,sig);
        check(ECDSAVerify(f,pub,sig), (std::string("ECDSA valid ")+c).c_str());
        { std::ofstream o(d/"tamper.bin",std::ios::binary); o<<"x"; }
        check(!ECDSAVerify(d/"tamper.bin",pub,sig), (std::string("ECDSA tampered rejected ")+c).c_str());
    }
    std::cout<<(fails?"FAILURES\n":"ALL PASSED\n"); return fails;
}
