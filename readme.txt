Ki2Key ( Kinect to Key ) version alpha 1 README
Tatsuhiko Suzuki, Aug. 20, 2011

The official website is http://vram.org/project.html

--
1. Summary
2. System requirements
3. Compilation and installation
4. Basic usage
5. Additional information
6. Disclaimer and license
--

1. Summary
 This software sends the keystrokes to your application by
 scanning your gestures. This scanning method uses OpenNI compliant devices.
 Since Ki2Key is under heavy development at the moment, it may cause unexpected
 crashes, instability and frequent specification changes. Therefore,
 It is suitable for users who have technical knowledge of computer software.

2. System requirements
 * Microsoft Windows XP SP2 or later
 * Kinect for Xbox360
   ( http://www.xbox.com/en-US/Xbox360/Accessories/Kinect/kinectforxbox360 )
 * OpenNI Stable Build for Windows x86 (32-bit) and PrimeSense NITE
   Stable Build for Windows x86 (32-bit)
   ( http://openni.org/downloadfiles/opennimodules )

 Please set up the software and hardwares as specified above, and then confirm
 that the sample programs of OpenNI and NITE can run properly. After that,
 you can install Ki2Key.

3. Compilation and installation
 * If downloading the binary package.
  You do not need to compile it. Simply copy the ki2key.exe file into the
  desired folder.

 * If downloading the source package of Ki2Key from github.
  Please open ki2key.sln by Microsoft Visual Studio 2008 Express, and
  compile and build suitable binaries as necessary. Then copy it into
  the desired folder.

4. Basic usage
 1. Double click the executable file ( ki2key.exe ) and run the software.
 2. Move your body to establish recognition by the sensing device. When the
    device recognizes you, the image of your body will be tinted with
    a specific color on the monitor.
 3. Make the device recognize your hand by waving your hand. When it recognizes
    your hand correctly, a square tile will be appear on the monitor. Confirm
    that the tile color changes when you poke the tile with your hand.
 4. Click the 'Add' button to add an 'action' item to the list.
 5. Click the 'N/A #1' on the left end of the item to start the gesture input
    mode. Poke the tile and move your hand before reaching count 0.
 6. Pull your hand to complete the gesture. An 'Action' item shows the gesture
    as moving 'U'p, 'D'own, 'L'eft and 'R'ight.
 7. You need to set the application that is sent a keystroke by your gesture.
    Click 'Undefined' on the center of the item, after that click the
    destination window of the application.
 8. An 'Action' item shows the process and class name of the application.
 9. You need to set the keystroke when you have gestured. Click 'None' on
    the right end of the item, and then press the desired key.
 10. An 'Action' item shows the key name.
 11. Confirm the action. Right click the 'Action' item to check whether
     the desired key will be sent to the destination application.
 12. The 'Action' is prepared. Poke the tile and move your hand in any
     of the predefined gestures in order to achieve the desired control
     function. Enjoy & Excitement!

5. Additional information
 If you find any bug or you would like an enhancement, please check
 the list of existing issues on the github repository before sending a
 report. If the issue not listed, report it using github issues.
 For the latest information, please refer to the web site of the
 Ki2Key project and the repository on github.
  * Ki2Key project ( http://vram.org/project.html )
  * Ki2Key repository on github ( https://github.com/szk/Ki2Key/ )

6. Disclaimer and license
 * Disclaimer
  The author of this software accept no liability for any damage/loss
  caused by the use/misuse of the software. These Terms of Use and
  License Agreement may be changed at any time without notice.
  All trademarks and registered trademarks are the property of their
  respective owners.

 * License
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

