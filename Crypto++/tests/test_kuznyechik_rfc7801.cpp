#include <iostream>
#include "hexToBytes.h"
#include "kuznyechik/kuznExpandKey.h"
#include "kuznyechik/kuznEncryptBlock.h"
#include "kuznyechik/kuznDecryptBlock.h"
static int fails=0;
static void check(bool ok,const std::string& n){ std::cout<<(ok?"[OK]   ":"[FAIL] ")<<n<<"\n"; if(!ok) fails++; }
using V=std::vector<uint8_t>;
static V h(const char* s){ return hexToBytes(s); }
static V run(void(*f)(uint8_t*), V v){ f(v.data()); return v; }
int main(){
    // таблица inverse из RFC 7801 (4.1) -- проверка transcribed pi
    const KuznyechikTables& T = kuznTables();
    const uint8_t piInvRfc[256] = {
 165,45,50,143,14,48,56,192,84,230,158, 57,85,126,82,145,100,3,87,90,28,96, 7,24,33,114,168,209,41,198,164,63,224,
 39,141,12,130,234,174,180,154,99,73,229, 66,228,21,183,200,6,112,157,65,117,25, 201,170,252,77,191,42,115,132,213,195,175,
 43,134,167,177,178,91,70,211,159,253,212, 15,156,47,155,67,239,217,121,182,83,127, 193,240,35,231,37,94,181,30,162,223,166,
 254,172,34,249,226,74,188,53,202,238,120, 5,107,81,225,89,163,242,113,86,17,106, 137,148,101,140,187,119,60,123,40,171,210,
 49,222,196,95,204,207,118,44,184,216,46, 54,219,105,179,20,149,190,98,161,59,22, 102,233,92,108,109,173,55,97,75,185,227,
 186,241,160,133,131,218,71,197,176,51,250, 150,111,110,194,246,80,255,93,169,142,23, 27,151,125,236,88,247,31,251,124,9,13,
 122,103,69,135,220,232,79,29,78,4,235, 248,243,62,61,189,138,136,221,205,11,19, 152,2,147,128,144,208,36,52,203,237,244,
 206,153,16,68,64,146,58,1,38,18,26, 72,104,245,129,139,199,214,32,10,8,0, 76,215,116 };
    bool inv=true; for(int i=0;i<256;i++) if(T.piInv[i]!=piInvRfc[i]) inv=false;
    check(inv,"RFC7801 4.1   pi / pi^-1 table (256 values)");
    // 5.1 S
    { V a=h("ffeeddccbbaa99881122334455667700");
      for (const char* e : {"b66cd8887d38e8d77765aeea0c9a7efc","559d8dd7bd06cbfe7e7b262523280d39","0c3322fed531e4630d80ef5c5a81c50b","23ae65633f842d29c5df529c13f5acda"}) {
        for(auto& b:a) b=T.pi[b]; check(a==h(e), std::string("RFC7801 5.1   S -> ")+e); } }
    // 5.2 R
    { V a=h("00000000000000000000000000000100");
      for (const char* e : {"94000000000000000000000000000001","a5940000000000000000000000000000","64a59400000000000000000000000000","0d64a594000000000000000000000000"}) {
        kuznR(a.data()); check(a==h(e), std::string("RFC7801 5.2   R -> ")+e); } }
    // 5.3 L
    { const char* in[4]={"64a59400000000000000000000000000","d456584dd0e3e84cc3166e4b7fa2890d","79d26221b87b584cd42fbc4ffea5de9a","0e93691a0cfc60408b7b68f66b513c13"};
      const char* ex[4]={"d456584dd0e3e84cc3166e4b7fa2890d","79d26221b87b584cd42fbc4ffea5de9a","0e93691a0cfc60408b7b68f66b513c13","e6a8094fee0aa204fd97bcb0b44b8580"};
      for(int i=0;i<4;i++){ V a=h(in[i]); kuznL(a.data()); check(a==h(ex[i]), std::string("RFC7801 5.3   L -> ")+ex[i]);
                            kuznLInv(a.data()); check(a==h(in[i]), "           L^-1(L(x)) == x"); } }
    // 5.4 constants
    { const char* c[8]={"6ea276726c487ab85d27bd10dd849401","dc87ece4d890f4b3ba4eb92079cbeb02","b2259a96b4d88e0be7690430a44f7f03","7bcd1b0b73e32ba5b79cb140f2551504",
                        "156f6d791fab511deabb0c502fd18105","a74af7efab73df160dd208608b9efe06","c9e8819dc73ba5ae50f5b570561a6a07","f6593616e6055689adfba18027aa2a08"};
      bool ok=true; for(int i=0;i<8;i++) if(V(T.c[i],T.c[i]+16)!=h(c[i])) ok=false; check(ok,"RFC7801 5.4   round constants C_1..C_8"); }
    // 5.4 key schedule
    V key=h("8899aabbccddeeff00112233445566778899aabbccddeeff"); 
    key=h("8899aabbccddeeff0011223344556677fedcba98765432100123456789abcdef");
    uint8_t rk[10][16]; kuznExpandKey(key.data(), rk);
    const char* K[10]={"8899aabbccddeeff0011223344556677","fedcba98765432100123456789abcdef","db31485315694343228d6aef8cc78c44","3d4553d8e9cfec6815ebadc40a9ffd04",
      "57646468c44a5e28d3e59246f429f1ac","bd079435165c6432b532e82834da581b","51e640757e8745de705727265a0098b1","5a7925017b9fdd3ed72a91a22286f984",
      "bb44e25378c73123a5f32f73cdb6e517","72e9dd7416bcf45b755dbaa88e4a4043"};
    bool ks=true; for(int i=0;i<10;i++) if(V(rk[i],rk[i]+16)!=h(K[i])) { ks=false; std::cout<<"   mismatch K_"<<i+1<<"\n"; }
    check(ks,"RFC7801 5.4   key schedule (10 round keys)");
    // 5.5 / 5.6
    V pt=h("1122334455667700ffeeddccbbaa9988"), ct=h("7f679d90bebc24305a468d42b9d4edcd"), o(16);
    kuznEncryptBlock(pt.data(), o.data(), rk); check(o==ct,"RFC7801 5.5   encryption");
    kuznDecryptBlock(ct.data(), o.data(), rk); check(o==pt,"RFC7801 5.6   decryption");
    std::cout<<(fails?"FAILURES\n":"KUZNYECHIK: ALL PASSED\n"); return fails;
}
