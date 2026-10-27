#include <algorithm>
#include <iostream>
#include "hexToBytes.h"
#include "toBinaryString.h"
#include "streebog/streebogInit.h"
#include "streebog/streebogUpdate.h"
#include "streebog/streebogFinal.h"
static int fails=0;
static void check(bool ok,const std::string& n){ std::cout<<(ok?"[OK]   ":"[FAIL] ")<<n<<"\n"; if(!ok) fails++; }
using V=std::vector<uint8_t>;
static V hashOf(const V& msg,int bits,size_t chunk=0){
    StreebogContext c; streebogInit(c,bits);
    if(!chunk) streebogUpdate(c,msg.data(),msg.size());
    else for(size_t i=0;i<msg.size();i+=chunk) streebogUpdate(c,msg.data()+i,std::min(chunk,msg.size()-i));
    V out(bits/8); streebogFinal(c,out.data()); return out; }
static V rev(V v){ std::reverse(v.begin(),v.end()); return v; }
static std::string hex(const V& v){ static const char* d="0123456789abcdef"; std::string s; for(auto b:v){s+=d[b>>4];s+=d[b&15];} return s; }
int main(){
    // сообщения из RFC записаны числами «старший байт первым»; в памяти байты идут в обратном порядке
    V M1 = rev(hexToBytes("323130393837363534333231303938373635343332313039383736353433323130393837363534333231303938373635343332313039383736353433323130"));
    V M2 = rev(hexToBytes("fbe2e5f0eee3c820fbeafaebef20fffbf0e1e0f0f520e0ed20e8ece0ebe5f0f2f120fff0eeec20f120faf2fee5e2202ce8f6f3ede220e8e6eee1e8f0f2d1202ce8f0f2e5e220e5d1"));
    std::cout<<"M1 = \""<<std::string(M1.begin(),M1.end())<<"\" ("<<M1.size()<<" bytes), M2 = "<<M2.size()<<" bytes\n";
    check(rev(hashOf(M1,512))==hexToBytes("486f64c1917879417fef082b3381a4e211c324f074654c38823a7b76f830ad00fa1fbae42b1285c0352f227524bc9ab16254288dd6863dccd5b9f54a1ad0541b"),"RFC6986 10.1.1  H512(M1)");
    check(rev(hashOf(M1,256))==hexToBytes("00557be5e584fd52a449b16b0251d05d27f94ab76cbaa6da890b59d8ef1e159d"),"RFC6986 10.1.2  H256(M1)");
    check(rev(hashOf(M2,512))==hexToBytes("28fbc9bada033b1460642bdcddb90c3fb3e56c497ccd0f62b8a2ad4935e85f037613966de4ee00531ae60f3b5a47f8dae06915d5f2f194996fcabf2622e6881e"),"RFC6986 10.2.1  H512(M2)");
    check(rev(hashOf(M2,256))==hexToBytes("508f7e553c06501d749a66fc28c6cac0b005746d97537fa85d9e40904efed29d"),"RFC6986 10.2.2  H256(M2)");
    // промежуточное значение: K = LPS(0^512) = b383fc2eced4a574 (повторяется 8 раз), little-endian
    { uint8_t z[64]={0}; streebogLPS(z,z); V k(z,z+64); check(rev(k)==hexToBytes("b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574b383fc2eced4a574"),"RFC6986 10.1.1  K = LPS(0^512)"); }
    // потоковость: любые куски дают тот же результат
    check(hashOf(M2,512,1)==hashOf(M2,512) && hashOf(M2,256,7)==hashOf(M2,256) && hashOf(M1,512,64)==hashOf(M1,512),"streaming == one-shot (chunk 1 / 7 / 64)");
    V big(1000003); for(size_t i=0;i<big.size();i++) big[i]=uint8_t(i*31+7);
    check(hashOf(big,512,4096)==hashOf(big,512) && hashOf(big,512,1000)==hashOf(big,512),"streaming == one-shot on 1 MB");
    V empty; std::cout<<"H256(empty) memory order = "<<hex(hashOf(empty,256))<<"\n";
    std::cout<<(fails?"FAILURES\n":"STREEBOG: ALL PASSED\n"); return fails;
}
