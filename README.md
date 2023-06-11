<img src="Resources/Icon128.png" align="right" width="110">

# Advanced Locomotion System Refactored

Completely reworked and improved С++ version of [Advanced Locomotion System V4](https://www.unrealengine.com/marketplace/en-US/product/advanced-locomotion-system-v1).

The main purpose of this ALS version is is a complete and high-quality refactoring of every aspect of the plugin, so that it becomes easier to understand, more flexible, reliable (especially in network multiplayer), performant, and simply more pleasant to work with.

## Features

- Refactored plugin structure. Content is now separated into 3 categories: `ALS` - main content, `ALSCamera` - camera-related content and `ALSExtras` - other optional content
>
- Use of multiple linked animation blueprints instead of a single monolithic animation blueprint.
>
- Use of animation layer interfaces to dynamically switch overlay states at runtime instead of referencing them directly in animation blueprints.
>
- Use of the **Control Rig** plugin for IK and other bone manipulation tasks instead of animation graph nodes.
>
- Significantly reduced number of virtual bones required by the plugin.
>
- Added the ability to disable the use of IK bones and use only virtual bones for all bone manipulation tasks, which can be useful in cases where IK bones are not animated in your custom animations.
>
- Added a skeleton asset action that can help set up all the necessary skeleton stuff: animation curves, slots, virtual bones, and so on. Can be accessed by right clicking on the skeleton asset -> Scripted Asset Actions -> Setup Als Skeleton.
>
- Reworked foot locking to make it more performant and reliable, especially in network multiplayer.
>
- Reworked foot and pelvis offset logic for smoother character movement on stairs or sloped surfaces.
>
- Added "Slot" layering animation curves that allow to control the blending of overlay poses with animation montages played inside "Layering" animation slots.
>
- Added `Blend Curves` and `Blend Poses by Gameplay Tag` animation graph nodes. `Blend Curves` allows to blend animation curves without the need for the `VB curves` virtual bone and `Blend Poses by Gameplay Tag` is similar to `Blend Poses by Enum` but uses gameplay tags instead of enum.
>
- Moved thread-safe animation instance logic to the worker thread to improve game thread performance.
>
- Replaced some C++ enums such as `EAlsOverlayMode` with gameplay tags so new entries can be added without the need to modify the code.
>
- Reworked `UAlsCharacterMovementComponent` for better movement synchronization over the network.
>
- Reworked mantling. Implemented as a root motion source for better movement synchronization over the network.
>
- Reworked camera. Implemented as a component similar to the standard camera component, no need for custom `APlayerCameraManager` or `APlayerController` classes.
>
- Reworked debug mode. Can be toggled by pressing `Shift + [1-8]` or using console commands such as `ShowDebug ALS.Curves`.
>
- Use of **Push Model** for more efficient replication.
>
- Use of **MetaSounds** and **Enhanced Input** plugins.
>
- Support of **Update Rate Optimization** (disabled by default) and **Large World Coordinates**.
>
- Most of the recent significant changes from the [Community Version](https://github.com/dyanikoglu/ALS-Community) are also present here.
>
- A lot of other fixes, improvements and refactorings.

Also see the [Releases](https://github.com/Sixze/ALS-Refactored/releases) page for more details.
Reading the changelogs is a good way to keep up to date with all the latest features that the plugin has to offer.

## Supported Unreal Engine Versions & Platforms

| Plugin Version                                                            | Unreal Engine Version           |
|---------------------------------------------------------------------------|---------------------------------|
| [4.12](https://github.com/Sixze/ALS-Refactored/releases/tag/4.12)         | 5.2                             |
| [4.11](https://github.com/Sixze/ALS-Refactored/releases/tag/4.11)         | 5.1                             |
| [4.10](https://github.com/Sixze/ALS-Refactored/releases/tag/4.10) - 4.9   | 5.0                             |
| [4.8](https://github.com/Sixze/ALS-Refactored/releases/tag/4.8) and below | 4.26, 4.27 and 5.0 Early Access |

**The plugin is mainly developed and tested on Windows, so use it on other platforms at your own risk.**

## Quick Start

1. Clone the repository to your project's `Plugins` folder, or download the latest release and extract it to your project's `Plugins` folder.
2. Merge the `DefaultEngine.ini` and `DefaultInput.ini` files from the plugin's `Config` folder with files from your project's `Config` folder.
3. Recompile your project.

## Known Issues & Discussion
- See [Issues](https://github.com/Sixze/ALS-Refactored/issues) section for list of known issues.
- See [Discussions](https://github.com/Sixze/ALS-Refactored/discussions) section to discuss anything about the plugin, and ask questions. Please do not open an issue for questions.


## License & Contribution

ALS Refactored is licensed under the MIT License, see [LICENSE.md](LICENSE.md) for more information. Other developers are encouraged to fork the repository, open issues & pull requests to help the development.