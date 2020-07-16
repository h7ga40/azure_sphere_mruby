# mruby device client for Azure Sphere

[Github リポジトリ](https://github.com/h7ga40/azure_sphere_mruby)

[Azure Sphere](https://azure.microsoft.com/ja-jp/services/azure-sphere/)対応のボード[MT3620 Mini Dev Board](https://wiki.seeedstudio.com/MT3620_Mini_Dev_Board/)に、[mruby](https://github.com/mruby/mruby)を入れて、Rubyでデバイスクライアントをコーディング出来るようにした例。

## ビルド

[Visual Studio Code](https://code.visualstudio.com/)を使います。
***必ず```start_code.bat```から起動***します。```PATH```などの環境変数の設定が必要なためです。

手元ではWindowsで開発していて、Linuxでも出来そうですがVSCodeの設定はしていません。

```mruby```と```app```フォルダがビルド対象で、```mruby```は「ビルドタスクの実行」（Ctrl+Shilf+B）でビルドします。```app```は「cmake: Build」（F7）でビルド出来ます。両方```CMake```で一括でビルド出来ないか調査中です。

## 開発環境

mrubyのビルドに、[Visual Studio 2019](https://visualstudio.microsoft.com/ja/vs/)と[Ruby](https://rubyinstaller.org/)が必要です。
Azure Sphereのビルド環境は、「[Azure Sphere SDK for Windows をインストールする](https://docs.microsoft.com/ja-jp/azure-sphere/install/install-sdk?pivots=vs-code)」を参考にインストールしてください。

## デバッグ

Azure Sphere を使い始めるには、クラウドへのデバイスの登録などの一連の作業が必要です。
[こちら](https://docs.microsoft.com/ja-jp/azure-sphere/install/overview)などにあるドキュメントの手順でデバッグが出来る状態にする必要があります。

ちなみに、プロキシ環境下では```192.168.35.*```を、Windowsの設定でプロキシを通さないようにする必要がありました。
