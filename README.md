# ledcomm

Visible light communication tool

## Summery

スマートフォンのフラッシュライトを用いた可視光通信における
フラッシュライトの点滅を受信するためのプログラム集

## Manifest

- README.md: このファイル

- Arduino/VLC_resolve_protocol/VLC_resolve_protocol.ino:
  - 起動するとフラッシュライトの点滅から文字列データを受信
    - リーダ: 点灯5T
    - データ: 消灯1T+点灯1T("0"のデータ)，消灯1T+点灯2T("1"のデータ)
    - トレイラ: 消灯5T以上
  　
- Arduino/receive_string/receive_string.ino:
  - 起動するとフラッシュライトの点滅から文字列データを受信
    - リーダ: 点灯8T+消灯4T
    - データ: 消灯1T+点灯1T("0"のデータ)，消灯1T+点灯2T("1"のデータ)
    - トレイラ: 消灯5T
    
- Arduino/receive_string_oldtype/receive_string_oldtype.ino:
  - 起動するとフラッシュライトの点滅から文字列データを受信
    - リーダ: 点灯1T
    - データ: 消灯1T+点灯1T("0"のデータ)，消灯1T+点灯2T("1"のデータ)
    - トレイラ: 消灯5T
