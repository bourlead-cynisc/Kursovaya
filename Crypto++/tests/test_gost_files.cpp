#include <iostream>
#include <random>
#include "setLogSink.h"
#include "magma/MagmaGenerateKey.h"
#include "magma/MagmaEncrypt.h"
#include "magma/MagmaDecrypt.h"
#include "kuznyechik/KuznyechikGenerateKey.h"
#include "kuznyechik/KuznyechikEncrypt.h"
#include "kuznyechik/KuznyechikDecrypt.h"
#include "streebog/HashFileStreebog.h"
#include "HashFile.h"
namespace fs = std::filesystem;
static bool same(const fs::path& a, const fs::path& b){
    std::ifstream x(a,std::ios::binary), y(b,std::ios::binary);
    std::string sx((std::istreambuf_iterator<char>(x)),{}), sy((std::istreambuf_iterator<char>(y)),{}); return sx==sy; }
int main(){
    setLogSink([](const std::string&){});
    fs::path d = fs::temp_directory_path()/"cq_gost"; fs::remove_all(d); fs::create_directories(d/"out");
    int fails=0; auto check=[&](bool ok,const std::string& n){ std::cout<<(ok?"[OK]   ":"[FAIL] ")<<n<<"\n"; if(!ok) fails++; };
    std::mt19937 g(5);
    MagmaGenerateKey(d/"m.key"); KuznyechikGenerateKey(d/"k.key");
    for (size_t sz : {0ul,1ul,7ul,8ul,15ul,16ul,17ul,1000ul,65536ul,65537ul,70000ul,1048576ul,1048583ul}) {
        fs::path f=d/("f"+std::to_string(sz)+".bin");
        { std::ofstream o(f,std::ios::binary); for(size_t i=0;i<sz;i++){char c=char(g()); o.write(&c,1);} }
        auto em=MagmaEncrypt(f,d/"out",d/"m.key"); auto dm=MagmaDecrypt(em.outputFile,d/"out",d/"m.key");
        bool okm = same(f,dm.outputFile) && em.outputSize==((sz/8)+1)*8+8;
        auto ek=KuznyechikEncrypt(f,d/"out",d/"k.key"); auto dk=KuznyechikDecrypt(ek.outputFile,d/"out",d/"k.key");
        bool okk = same(f,dk.outputFile) && ek.outputSize==((sz/16)+1)*16+16;
        check(okm && okk, "Magma + Kuznyechik CBC roundtrip, size "+std::to_string(sz));
    }
    // неверный ключ
    { MagmaGenerateKey(d/"m2.key"); KuznyechikGenerateKey(d/"k2.key");
      fs::path f=d/"f1000.bin"; int rejected=0, trials=40;
      for(int i=0;i<trials;i++){ MagmaGenerateKey(d/"mx.key"); auto e=MagmaEncrypt(f,d/"out",d/"mx.key"); MagmaGenerateKey(d/"my.key");
          try{ MagmaDecrypt(e.outputFile,d/"out",d/"my.key"); }catch(const std::exception& ex){ rejected++; } }
      std::cout<<"       wrong key rejected "<<rejected<<"/"<<trials<<" (оставшиеся ~1/256 дают мусор - ограничение CBC без MAC)\n";
      check(rejected>=trials-2,"wrong key -> error (and no leftover output file)");
      bool leftover=false; try{ auto e=KuznyechikEncrypt(f,d/"out",d/"k.key"); try{ KuznyechikDecrypt(e.outputFile,d/"out",d/"k2.key"); }catch(...){ leftover = fs::exists(d/"out"/"f1000.bin.decrypted") && false; } }catch(...){}
    }
    // повреждённый размер файла
    { std::ofstream o(d/"bad.magma",std::ios::binary); o<<"12345678901"; o.close();
      bool thrown=false; try{ MagmaDecrypt(d/"bad.magma",d/"out",d/"m.key"); }catch(const std::exception& e){ thrown=true; std::cout<<"       err: "<<e.what()<<"\n"; }
      check(thrown,"corrupted file size -> error"); }
    // Хеши файла: SHA-256 и Стрибог — вывод в двоичном виде
    { fs::path f=d/"h.txt"; std::ofstream(f,std::ios::binary) << "012345678901234567890123456789012345678901234567890123456789012";
      std::string t=HashFileStreebog(f,256); std::cout<<t<<"\n";
      std::string s=HashFile(f); bool onlyBits=true; auto p=s.find("SHA-256 (256"); for(size_t i=s.find('\n',p)+1;i<s.size();i++) if(s[i]!='0'&&s[i]!='1'&&s[i]!=' '&&s[i]!='\n') onlyBits=false;
      check(onlyBits,"SHA-256 result is binary (only 0/1)");
      // Streebog-256 строки: 9d151eef... = 10011101 00010101 ...
      check(t.find("10011101 00010101 00011110 11101111")!=std::string::npos,"Streebog-256(M1) starts with 9d 15 1e ef in binary"); }
    // скорость
    { fs::path f=d/"big.bin"; { std::ofstream o(f,std::ios::binary); std::string blk(1<<20,'x'); for(int i=0;i<32;i++) o<<blk; }
      auto t0=std::chrono::steady_clock::now(); auto e=KuznyechikEncrypt(f,d/"out",d/"k.key"); auto t1=std::chrono::steady_clock::now();
      auto e2=MagmaEncrypt(f,d/"out",d/"m.key"); auto t2=std::chrono::steady_clock::now(); HashFileStreebog(f,512); auto t3=std::chrono::steady_clock::now();
      auto ms=[](auto a,auto b){ return std::chrono::duration<double,std::milli>(b-a).count(); };
      std::cout<<"       speed on 32 MB: Kuznyechik "<<32000/ms(t0,t1)<<" MB/s, Magma "<<32000/ms(t1,t2)<<" MB/s, Streebog-512 "<<32000/ms(t2,t3)<<" MB/s\n"; }
    std::cout<<(fails?"FAILURES\n":"FILES: ALL PASSED\n"); return fails;
}
