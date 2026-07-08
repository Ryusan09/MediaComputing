# 第11回 教師なし学習と教師あり学習

## 内容

- k-means 法によるクラスタリング
- 画像の色ベクトルを使ったクラスタリング
- SVM による教師あり分類
- HOG 特徴量を使った画像分類

## ファイル構成

| パス | 内容 |
|---|---|
| `MC2026-11.pdf` | 第11回講義資料 |
| `演習1/mc11-1.cpp` | 2次元点群の k-means クラスタリング |
| `演習2/mc11-2.cpp` | 画像の画素値を k-means で減色 |
| `演習3/mc11-3.cpp` | 花画像を色比率でクラスタリングする演習 |
| `演習4/mc11-4.cpp` | HOG 特徴量と SVM による画像分類 |
| `kadai/mc11-kadai.cpp` | 課題: 飲み物とスイーツの写真分類 |

## 課題

`kadai/trainImage` の学習画像から HOG 特徴量を計算し、SVM で `drink` と `sweets` を分類する。
`kadai/testImage` の2枚について、分類結果を標準出力に表示し、ラベル付き画像を `kadai/result_kadai.jpg` に保存する。

```sh
cd 11/kadai
g++ mc11-kadai.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
./a.out
```

GUIで確認する場合:

```sh
./a.out --show
```
