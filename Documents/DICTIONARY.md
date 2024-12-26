# x86ドキュメント (個人的メモ)
## Instructionsの大枠について
* 以下構成を取る
  * **Prefix**:       0xf0, 0xf2, 0xf3, 0x26, 0x2e, 0x36, 0x3e, 0x64-67のいずれか
  * **ModR/M**:       レジスタ・アドレッシングモードの指定，オペランドの拡張に用いる
  * **SIB**:          複雑なアドレス計算に用いる．(例: [eax + 4*ecx + 123])
    * ss/iii/bbbな構造をとる
      * [scale * index(reg) + base]で計算．
      * scaleは0, 2, 4, 8をとり，それぞれ00, 01, 10, 11
      * indexはそのままレジスタ
      * baseは000~111をとり，レジスタであったりディスプレースメントと組み合わせたり変わる．
  * **displacement**: アドレスの差分．(例: [ebp - 4]の-4)
    ```c
    prefix | opecode | ModR/M | SIB | displacement | immediate
      0~4  |   1~3   |  0~1   | 0~1 |  0, 1, 2, 4  |  0,1,2,4  [byte]
    ```
### Mod R/Mとは
* Mod, REG, R/Mを持つ1バイトのこと
  * Mod + R/Mでレジスタとアドレッシングモードを決定(例: [ebp + disp32]など)
  * REGではもう一つのレジスタを決定
    |EAX|ECX|EDX|EBX|ESP|EBP|ESI|EDI|
    |-|-|-|-|-|-|-|-|
    |000|001|010|011|100|101|110|111|
  * どちらが第一/第二オペランドかはオペコードの仕様による
* mov eax, 0x78の場合
    * デコード結果はb8 78 00 00 00
    * ビッグエンディアンなので78(=0111 1000)は以下の通りとなる
        ```c
        7 6   5 4 3   2 1 0 [bit]
        0 0 | 0 1 0 | 0 0 0     // 1bitずつ逆順
        Mod |  REG  |  R/M
        ```

## 各命令についてのメモ
* 気が向いたらまとめる

## 詰まったメモ
* crtとcソースのリンク順を間違えて，時間とかした
* nop未実装だったからその影響か...?と勘違いした結果，0x66の16bit拡張を新規実装できたし，良しとする...
  * cソース→crt
  ```c
  ndisasm -b 32 test4
  00000000  55                push ebp  // <- 恐らく原因はこいつで，何をpushしているんだといったところ
  00000001  89E5              mov ebp,esp
  00000003  83EC10            sub esp,byte +0x10
  00000006  C745FC28000000    mov dword [ebp-0x4],0x28
  0000000D  8345FC01          add dword [ebp-0x4],byte +0x1
  00000011  8B45FC            mov eax,[ebp-0x4]
  00000014  C9                leave
  00000015  C3                ret
  00000016  6690              xchg ax,ax
  00000018  6690              xchg ax,ax
  0000001A  6690              xchg ax,ax
  0000001C  6690              xchg ax,ax
  0000001E  6690              xchg ax,ax
  00000020  E8DBFFFFFF        call 0x0
  00000025  E9D683FFFF        jmp 0xffff8400
  * その結果
  ```
  ```c
  EIP = 7c00, Code = 55
  EIP = 7c01, Code = 89
  EIP = 7c03, Code = 83
  EIP = 7c06, Code = c7
  EIP = 7c0d, Code = 83
  EIP = 7c11, Code = 8b
  EIP = 7c14, Code = c9
  EIP = 7c15, Code = c3
  [!] Segmentation fault: accessed out of memory
  stop pc = 83e58955  // <- こいつがおかしかった

  EAX = 00000029
  ECX = 00000000
  EDX = 00000000
  EBX = 00000000
  ESP = 00007c04
  EBP = 00000000
  ESI = 00000000
  EDI = 00000000
  EIP = 83e58955
  ```
  * crt→cソース
  ```c
  ndisasm -b 32 test4
  00000000  E805000000        call 0xa
  00000005  E9F683FFFF        jmp 0xffff8400
  0000000A  55                push ebp
  0000000B  89E5              mov ebp,esp
  0000000D  83EC10            sub esp,byte +0x10
  00000010  C745FC28000000    mov dword [ebp-0x4],0x28
  00000017  8345FC01          add dword [ebp-0x4],byte +0x1
  0000001B  8B45FC            mov eax,[ebp-0x4]
  0000001E  C9                leave
  0000001F  C3                ret
  ```
  * その結果
  ```c
  EIP = 7c00, Code = e8
  EIP = 7c0a, Code = 55
  EIP = 7c0b, Code = 89
  EIP = 7c0d, Code = 83
  EIP = 7c10, Code = c7
  EIP = 7c17, Code = 83
  EIP = 7c1b, Code = 8b
  EIP = 7c1e, Code = c9
  EIP = 7c1f, Code = c3
  EIP = 7c05, Code = e9
  stop pc = 00

  EAX = 00000029
  ECX = 00000000
  EDX = 00000000
  EBX = 00000000
  ESP = 00007c00
  EBP = 00000000
  ESI = 00000000
  EDI = 00000000
  EIP = 00000000
  ```