# Advanced Locomotion System V4 Refactored
![Image](https://github.com/Sixze/ALSRefactored/raw/main/Screenshots/Readme_Content_2.gif)

Completely refactored and bug fixed version of the C++ port of [Advanced Locomotion System V4](https://www.unrealengine.com/marketplace/en-US/product/advanced-locomotion-system-v1) for **Unreal Engine 4.26**.

## Supported Platforms
- Windows
- Linux

*Mac, Android, IOS, and console builds are not tested and supported at the moment. Use the plugin on those platforms with your own risk.*

## Features
- Full replication support with low bandwidth usage
- Plugin structure
- Highly optimized for production
- Lots of bug fixes additional to marketplace version

## Known Issues & Discussion
- See [Issues](https://github.com/Sixze/ALSRefactored/issues) section for list of known issues
- See [Discussions](https://github.com/Sixze/ALSRefactored/discussions) section to discuss anything about the plugin, and ask questions. Please do not open an issue to ask questions about the plugin.

## Setting Up The Plugin
- **C++ project is a requirement. BP projects are currently not supported.**
- Clone the repository inside your project's `Plugins` folder, or download the latest release and extract it into your project's `Plugins` folder.
- Put `Config/DefaultInput.ini` from the plugin folder inside your project's config folder. If your project already have this .ini file, merge it into yours.
- Add the lines below into your `DefaultEngine.ini`, below `[/Script/Engine.CollisionProfile]` tag (create the tag if it doesn't exist):
```
+DefaultChannelResponses=(Channel=ECC_GameTraceChannel1,DefaultResponse=ECR_Block,bTraceType=True,bStaticObject=False,Name="AlsClimbable")
+Profiles=(Name="AlsPawn",CollisionEnabled=QueryAndPhysics,bCanModify=True,ObjectTypeName="Pawn",CustomResponses=((Channel="Visibility",Response=ECR_Ignore),(Channel="Camera",Response=ECR_Ignore)),HelpMessage="")
```
- Regenerate visual studio project files and build your project.
- Launch the project, and enable plugin content viewer as seen below. This will show contents of the plugin in your content browser:
![image](https://github.com/Sixze/ALSRefactored/raw/main/Screenshots/Readme_Content_1.png)

## License & Contribution
**Source code** of the plugin is licensed under MIT license, and other developers are encouraged to fork the repository, open issues & pull requests to help the development.
