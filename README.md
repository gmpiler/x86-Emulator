# About GEmu
* A tiny x86 Emulator.
## spec
### 全体 (gemu.c/h)
* GEmuはレジスタとメモリの状態をエミュレートする．
* mainではエミュレータ上で動かすプログラムを予め読み出し，その後1命令ずつ実行していく．
    * OutOfOrderなどは実装せず，逐次的にプログラムを実行する．
* 各命令では，prefix / opecodeの値に従い，命令関数を呼び出しレジスタやメモリステートの更新を行う．
    * つまり，**prefix / opecodeを解析する責任**を負う
### 命令関数 (instructions.c/h)
* 各関数では，pcがprefix / opecodeを指している状態から開始し，次の命令を指した状態で終了する．
* 関数内部で**ModR/M以降をデコードし，実行する責任**を負う
