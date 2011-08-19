Ki2Key ( Kinect to Key ) version alpha 1 README
2011/8/20 Tatsuhiko Suzuki

公式サイト: http://vram.org/project.html

--
1. 概要
2. 動作環境
3. コンパイル&インストール方法
4. 基本的な使い方
5. 追加情報
6. 免責事項&ライセンス
--

1. 概要
  ユーザーの手の動きをOpenNI対応のセンサー機器で読みとり、キーボードの
 キー入力に変換してアプリケーションへ送るソフトウェアです。
  現在のところ開発中のため、バグ、クラッシュや仕様変更が頻繁に発生します。
 コンピュータに関する専門的知識を持った方のみ使用してください。

2. 動作環境
 * Kinect for Xbox360
 ( http://www.xbox.com/en-US/Xbox360/Accessories/Kinect/kinectforxbox360 )
 * Windows XP SP2以降またはWindows 7の動作するPC
 * OpenNI 32bit版, NITE 32bit版 (OpenNI Stable Build for Windows x86
 (32-bit), PrimeSense NITE Stable Build for Windows x86 (32-bit))
 ( http://openni.org/downloadfiles/opennimodules )

   上記のソフトウェアとハードウェアをPCへインストールし、OpenNIとNITEの
  サンプルプログラムが正常に動く状態にしてからKi2Keyをインストールして
  ください。

3. コンパイル&インストール方法
 * Ki2Keyの実行ファイルパッケージをダウンロードした場合
   コンパイルする必要はありません。好きなフォルダに実行ファイル
  をコピーしてください。

 * Ki2Keyのソースパッケージをgithubからダウンロードした場合
   Microsoft Visual Studio 2008 Expressでki2key.slnを開き、適宜
  コンパイルして実行ファイルをビルドしてください。
  生成された実行ファイルを好きなフォルダにコピーしてください。

4. 基本的な使い方
 1. 実行ファイル( Ki2Key.exe )をダブルクリックし、ソフトウェアを起動します。
 2. モニター画像に姿が映るように移動して、身体を認識させてください。
    センサーが身体を認識すると、モニター画像へ映った姿に色がつきます。
 3. センサーに向かって手を振り、手を認識させてください。
    センサーが正常に認識すると、四角形のタイルがモニター画像へ表示されます。
    タイルの真後ろから手を差し出すとタイルの色が変わることを確認します。
 4. ウィンドウの右下にあるAddボタンを押して、リストにアクションを追加します。
 5. リストの左端にある"N/A #1"をクリックするとカウントダウンが始まります。
    カウントが0になる前にタイルの真後ろから手を差し出し、手を上下左右
    に動かしてジェスチャーを入力してください。
 6. ジェスチャーを完了するには、手をタイルより手前に引いてください。動
    きをU = Up(上)、D = Down(下)、L = Left(左)、R = Right(右)として
    リストの左端に表示します。
 7. ジェスチャーをした際にキー入力を送るアプリケーションを決めます。リ
    ストの中央にある"Undefined"をクリックした後、目的のアプリケーショ
    ンのウィンドウをクリックしてください。
 8. アプリケーションのプロセス名とクラス名がリストの中央に表示されます。
 9. ジェスチャーをした際に送るキー入力を決めます。リストの右端にある
    "None"をクリックした後、目的のキーを押してください。
 10. キー名がリストの右端に表示されます。
 11. 動作確認をします。今入力したリストの行を右クリックして、意図した
     通りにアプリケーションへキー入力が送られることを確認します。
 12. 準備が完了しました。タイルの真後ろから手を差し出し、先ほど入力し
     たジェスチャーをするとアプリケーションへキー入力を送ります。
     エンジョイ&エキサイティング!

5. 追加情報
 * バグを発見した場合は、githubで既に報告されていないか確かめた上、
  github上のissueを使って報告してください。その他の最新情報はプロジェク
  トのWebサイトとgithub上の開発用リポジトリを参照してください。
  * プロジェクトのWebサイト ( http://vram.org/project.html )
  * github上の開発用リポジトリ ( https://github.com/szk/Ki2Key/ )

6. 免責事項&ライセンス
 * 免責事項
  本ソフトウェアを使用したことによる損失や損害等について、ソフトウェア
  製作者はいかなる場合も一切の責任を負いません。
  ソフトウェアの仕様および本ドキュメントの内容は将来予告なしに変更する場合が
  あります。
  本ドキュメントに記載されている商品名、製品名、会社名などの固有名詞は、
  それぞれ各社の登録商標または商標です。

 * ライセンス
  Copyright (c) 2011, Tatsuhiko Suzuki
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of the copyright holders nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

