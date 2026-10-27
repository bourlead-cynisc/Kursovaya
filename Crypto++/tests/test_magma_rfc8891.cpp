#include <iostream>
#include "hexToBytes.h"
#include "magma/magmaExpandKey.h"
#include "magma/magmaEncryptBlock.h"
#include "magma/magmaDecryptBlock.h"
static int fails=0;
static void check(bool ok,const char* n){ std::cout<<(ok?"[OK]   ":"[FAIL] ")<<n<<"\n"; if(!ok) fails++; }
int main(){
    // A.1 transformation t
    check(magmaT(0xfdb97531)==0x2a196f34 && magmaT(0x2a196f34)==0xebd9f03a && magmaT(0xebd9f03a)==0xb039bb3d && magmaT(0xb039bb3d)==0x68695433,"RFC8891 A.1  t");
    // A.2 transformation g
    check(magmaG(0x87654321,0xfedcba98)==0xfdcbc20c && magmaG(0xfdcbc20c,0x87654321)==0x7e791a4b
       && magmaG(0x7e791a4b,0xfdcbc20c)==0xc76549ec && magmaG(0xc76549ec,0x7e791a4b)==0x9791c849,"RFC8891 A.2  g");
    auto key = hexToBytes("ffeeddccbbaa99887766554433221100f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
    uint32_t rk[32]; magmaExpandKey(key.data(), rk);
    uint32_t exp[32]={0xffeeddcc,0xbbaa9988,0x77665544,0x33221100,0xf0f1f2f3,0xf4f5f6f7,0xf8f9fafb,0xfcfdfeff,
                      0xffeeddcc,0xbbaa9988,0x77665544,0x33221100,0xf0f1f2f3,0xf4f5f6f7,0xf8f9fafb,0xfcfdfeff,
                      0xffeeddcc,0xbbaa9988,0x77665544,0x33221100,0xf0f1f2f3,0xf4f5f6f7,0xf8f9fafb,0xfcfdfeff,
                      0xfcfdfeff,0xf8f9fafb,0xf4f5f6f7,0xf0f1f2f3,0x33221100,0x77665544,0xbbaa9988,0xffeeddcc};
    bool ks=true; for(int i=0;i<32;i++) if(rk[i]!=exp[i]) ks=false;
    check(ks,"RFC8891 A.3  key schedule (32 round keys)");
    auto pt = hexToBytes("fedcba9876543210"), ct = hexToBytes("4ee901e5c2d8ca3d");
    uint8_t out[8]; magmaEncryptBlock(pt.data(), out, rk);
    check(std::vector<uint8_t>(out,out+8)==ct,"RFC8891 A.4  encryption");
    magmaDecryptBlock(ct.data(), out, rk);
    check(std::vector<uint8_t>(out,out+8)==pt,"RFC8891 A.5  decryption");
    std::cout<<(fails?"FAILURES\n":"MAGMA: ALL PASSED\n"); return fails;
}
