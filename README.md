# robosys2020_led
ロボットシステム学_課題1

## 概要
LEDの点滅と圧電ブザーの音でリズムをとることが出来るメトロノームのデバイスドライバです。  
ロボットシステム学講義内で上田先生が作成したデバイスドライバを改変して作成しています。  
  
## 動画
実際に使用している動画です。  
https://youtu.be/9eTYkHwrX5k  

## 必要なもの
Raspberry Pi 4B (Ubuntu 20.04 LTS)  
LED 5つ (端のみ色が違うとらしくなります)  
圧電ブザー 1つ  
抵抗(220Ω) 5つ  
それらをつなげる環境(ブレッドボードなど)  

## 回路について
動画で並べてあるLEDはそれぞれ左からそれぞれGPIO20,25,24,23,21につないであり、それぞれGPIOとLEDの間に220Ω抵抗が繋いであります。  
圧電ブザーはGPIO22に直接つないであります。  
マイナス側はブレッドボードの端でまとめてGNDにつないでいます。  
https://photos.app.goo.gl/tzMGLSzd3Rx25mF99  
少しわかりづらいですが回路を上から撮った写真です。  

## 動作方法
インストール  
以下のコマンドを実行します。  
  
```
$ git clone https://github.com/ventcoursiur/robosys2020_led.git  
$ cd robosys2020_led  
$ make  
$ sudo insmod myled.ko  
$ sudo chmod 666 /dev/myled0  
```
1. LEDのオンオフ  
echoを使って入力することで簡単なledの操作ができます。  
`$ echo ? > /dev/myled0 ## ?に対応する数字一文字を入れる`  
全LED点灯 → 9  
全LED点灯 → 0  
？秒ごとに一個ずつLEDを点灯。  
  
2. メトロノーム機能  
echoを使って入力することでメトロノームとして使えます。  
`$ echo ? > /dev/myled0 ## ?に対応する小文字アルファベット一文字を入れる`  
アルファベットとbpmの対応  
  ```
  a = 10bpm		k = 110bpm		u = 210bpm  
  b = 20bpm		l = 120bpm		v = 220bpm  
  c = 30bpm		m = 130bpm		w = 230bpm  
  d = 40bpm		n = 140bpm		x = 240bpm  
  e = 50bpm		o = 150bpm		y = 250bpm  
  f = 60bpm		p = 160bpm		z = 260bpm  
  g = 70bpm		q = 170bpm  
  h = 80bpm		r = 180bpm  
  i = 90bpm		s = 190bpm  
  j = 100bpm		t = 200bpm
  ```
例)　kならば110bpmが流れる  
  
## ライセンス
GNU General Public License v3.0  
リポジトリ内にあるCOPYINGを参照してください。  
