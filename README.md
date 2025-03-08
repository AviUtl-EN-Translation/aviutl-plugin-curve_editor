# Curve Editor for AviUtl

![GitHub](https://img.shields.io/github/license/mimaraka/aviutl-plugin-curve_editor)
![GitHub language count](https://img.shields.io/github/languages/count/mimaraka/aviutl-plugin-curve_editor)
![GitHub top language](https://img.shields.io/github/languages/top/mimaraka/aviutl-plugin-curve_editor)
![GitHub issues](https://img.shields.io/github/issues/mimaraka/aviutl-plugin-curve_editor)
![GitHub all releases](https://img.shields.io/github/downloads/mimaraka/aviutl-plugin-curve_editor/total)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/mimaraka/aviutl-plugin-curve_editor)

![thumbnail](https://github.com/user-attachments/assets/fa8243c9-3b7b-4b86-aa59-c266e6101fb7)

This is AviUtl's plugin and script that allows you to create and manage various easing.

## Download

Please download the latest version of the ZIP file ('curve_editor-v*.zip') from the link below.

[**Download**](https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/latest)

## Install

Unzip the downloaded ZIP file and place the following contents in the appropriate location.

|                     Content                     |                                                                 Move to                                                                  |
| :--------------------------------------------: | :-------------------------------------------------------------------------------------------------------------------------------------: |
| `curve_editor.auf`<br>`curve_editor`(folder) | directly under the AviUtl directory / `Plugins` folder / subfolder of `Plugins` (both should be placed in the same location).
|              `@Curve Editor.tra`               |                                             Inside the `script` folder or a subfolder within it.                                              |
|               `curve_editor.lua`               |                                                       Inside the folder where `exedit.auf` is located.                                                   |

![installation](https://github.com/user-attachments/assets/8edff440-b22f-45fc-a930-cac3285cd805)

> [!NOTE]
> When updating, please overwrite and place all files and folders.

> [!NOTE]
> If it is not working properly, please make sure that the programs below are installed on your computer.
>
> - [Visual C++ 2015/2017/2019/2022 Redistributable version (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 Runtime (x86)](https://developer.microsoft.com/ja-jp/microsoft-edge/webview2/?form=MA13LH)

## How to use

After starting AviUtl, click "Curve Editor" on the "Window" menu in the main window, and the Curve Editor window appears. ([→ If the window does not appear](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0))

This plugin allows you to edit curves in the following five editing modes.

- **Normal** (former: Bezier(Multiple))
- **Bezier**
- **Elastic**
- **Bounce**
- **Script**

You can switch between these modes from the mode selection dropdown in the top left corner of the window.

- #### Normal (former: Bezier(Multiple))

  You can save up to 524288 curves to a project file using IDs from `1` to `524288`.

  In this mode, you can create a curve by freely combining four types of curves: Linear, Bezier, Elastic, and Bounce.
  Double-click on the empty part of the graph to add an anchor.
  Double-click the anchor to delete the curve.
  
  Right-click on the anchor to change the curve type for each section or to set different modifiers for each section.

- #### Bezier

  A mode that allows you to create a single third-order bezier curve.
  The curve is identified by an integer value of several digits, and the value and curve correspond to one-to-one.

- #### Elastic

  A mode that allows you to create a movement such as rubber vibrations.
  Identifies curves by integers, similar to Bezier mode. Negative values will reverse the curve.

- #### Bounce

  A mode that allows you to create a movement in which objects bounce.
  Identifies curves by integers, similar to Bezier mode. Negative values will reverse the curve.

- #### Script

  A mode that allows you to describe the function of easing directly through a Lua script.
  Like the Normal mode, you can add up to 524288 curves.

### Graph View

You can zoom in and out of the view by rotating the mouse wheel.

You can change the view position by dragging the mouse wheel or dragging the left button while pressing the `Alt` key.

### Drag & Drop

When the 「Apply」 button is pressed, the button changes to the 「Apply by dragging and dropping onto the trackbar」. 
Drag the trackbar to the trackbar Movement Change Button without letting go of the left mouse button as it is.

When the cursor hover over the Movement Change Button of trackbar , the button is highlighted.
If you drop the curve over the track bar you want to apply it, the curve is applied.

[→ Drag & Drop is not working](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki/%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0)

> [!TIP]
> By dragging and dropping with the Shift key, you can set the easing individually for each coordinate even for the track bar where the easing, such as the XYZ coordinates, is set at once.

## Other

See [Wiki](https://github.com/mimaraka/aviutl-plugin-curve_editor/wiki) for more information on how to use it or work around common problems.

## Operating Environment

|      OS      | AviUtl | Advanced edit |
| :----------: | :----: | :------: |
| Windows 7~11 |  1.10  |   0.92   |

> [!IMPORTANT]
> Installation of the following programs is required.
>
> - [Visual C++ 2015/2017/2019/2022 Redistributable version (x86)](https://aka.ms/vs/17/release/vc_redist.x86.exe)
> - [Microsoft Edge WebView2 Runtime (x86)](https://developer.microsoft.com/en-us/microsoft-edge/webview2?form=MA13LH)

## Bug Report

I don't know if the bug in english version is a problem in the original version, so I won't write the link to the original bug report

## Disclaimer

he manufacturer is not responsible for any damages caused by the use of such a plugin and a script.
