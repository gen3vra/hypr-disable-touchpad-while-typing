# hypr-disable-touchpad-while-typing
Disable the touchpad while typing in Hyprland. Fixes palms messing with any touchpad input if working with the trackpad. Adjust the TIMEOUT_MS if needed.

## Help
- Use `hyprctl devices` to find trackpad device ID (may be under mice)
- Replace device ID with found device (like elan0406:00-04f3:31a6-touchpad)
- Replace keyboard device with your keyboard by using `ls /dev/input/by-id/`

## Compile
gcc -o typingtpblock typingtpblock.c

## Install (Debian)
mv ./typingtpblock /usr/local/bin/

/home/user/.config/systemd/user/typingtpblock.service
```
[Unit]
Description=Touchpad Auto-disable on Keyboard Input
After=hyprland-session.target

[Service]
Type=simple
ExecStart=/usr/local/bin/typingtpblock
Restart=on-failure

[Install]
WantedBy=hyprland-session.target