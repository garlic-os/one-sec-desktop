# One Sec Desktop
Qt port of [One Sec](https://one-sec.app/) by [riedel.wtf GmbH](https://riedel.wtf/)

Wanted a desktop version of One Sec. Didn't exist. So I made it!

```
Usage: onesec.exe [options] <program> [arguments]

Options:
  -?, -h, --help     Displays help on commandline options.
  --help-all         Displays help including Qt specific options.
  -v, --version      Displays version information.
  -n, --name <name>  Optional name for the program.
```

## Compiling
Made for Windows in Qt Creator 12.0 with Qt 6.5.3 and MSVC2019. Other configurations/OSes are surely possible just with other configurations.  
Uh sorry I don't actually know how to configure Qt Creator's Projects section for other people so just bear with me
1. Open the project in Qt Creator
2. Add a kit for whatever Qt6 version, compiler, and architecture you want to compile it for
3. Add Build Step: Custom Process Step
    - Command: `<Qt directory>\<version>\<compiler>\bin\windeployqt.exe`
    - Arguments: `--force --dir %{buildDir}\dist %{buildDir}\release\one-sec.exe`
    - Working directory: `%{buildDir}`
4. Add Build Step: Custom Process Step
    - Command: `cmd`
    - Arguments: `/c "copy %{buildDir}\release\one-sec.exe %{buildDir}\dist\onesec.exe"`
    - Working directory: `%{buildDir}` (Copy File doesn't let you use ${buildDir})
5. Press the hammer button and Qt Creator will compile the project to 
## Missing features
- Themes (I think): the website demo shows a different gradient for Instagram and there seems to be code in One Sec proper for other themes. But in Android and Desktop I've only ever seen the one dark theme, so that's all this has.
- Localization: only English. Sorry
- I don't think I got the animation timings and curves perfect
