# 第12回 課題 — 人の顔以外のカスケード分類器

## 選んだ分類器 その1: 猫の正面顔
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

## 選んだ分類器 その2: 車両検出
GitHubで公開されている車両検出用Haarカスケード `cars.xml`
（作者: Andrews Sobral、出典: https://github.com/andrewssobral/vehicle_detection_haarcascades ）
旧形式(haartraining)のXMLだが OpenCV 4.13 の `CascadeClassifier` でそのまま読み込めた。

```sh
g++ -std=c++11 mc12-kadai2.cpp `pkg-config --cflags --libs opencv4` -o mc12-kadai2
./mc12-kadai2 images/highway.jpg   # 結果は images/carResult.jpg
```

### 結果と考察
- 高速道路を見下ろした画像 `images/highway.jpg`（Wikimedia: I-80_Eastshore_Fwy.jpg）で18台検出。
  中〜遠距離の車は前面・背面ともよく検出できた（結果: `images/highwayResult.jpg`）。
- 渋滞の近接画像 `images/traffic.jpg`（Wikimedia: Trafficjam.jpg）では車の一部にしか枠が付かず2件のみ
  （結果: `images/trafficResult.jpg`）。この分類器は走行映像の中〜小サイズの車で学習されており、
  画面いっぱいの近接車両には合わない。猫顔の Cat03.jpg と同様、
  Haarカスケードは「学習時の見え方(スケール・視点)」から外れると急激に性能が落ちることを再確認した。
- パラメータはここでも scaleFactor=1.1 では検出0件で、1.05 への引き下げが必須だった。
