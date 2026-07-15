# 第12回 課題 — 人の顔以外のカスケード分類器

## 選んだ分類器
OpenCV 公式配布の猫の正面顔検出器 `haarcascade_frontalcatface_extended.xml`
（作者: Joseph Howse、出典: https://github.com/opencv/opencv/tree/master/data/haarcascades ）

## ビルド・実行
```sh
g++ -std=c++11 mc12-kadai.cpp `pkg-config --cflags --libs opencv4` -o mc12-kadai
./mc12-kadai images/cat2.jpg   # 静止画: 結果を images/catResult.jpg に保存
./mc12-kadai                   # 引数なしで内蔵カメラからリアルタイム検出 (q で終了)
```

## 結果と考察
- `images/cat2.jpg`（Wikimedia Commons: Felis_catus-cat_on_snow.jpg）で猫顔を1件正しく検出。結果は `images/catResult.jpg`。
- 人の顔用と同じパラメータ（scaleFactor=1.1, minNeighbors=3）では猫顔を取り逃した。
  scaleFactor を 1.02 に下げると検出できるが、しっぽへの誤検出も発生。
  minNeighbors を 8 に上げることで誤検出だけを除去できた。
- `images/cat.jpg`（Cat03.jpg、顔が画面いっぱいの傾いたアップ）は縮小・パラメータ調整でも検出できず。
  Haar カスケードは学習時の正面向きスケール構図から外れた画像に弱いことを確認した。
