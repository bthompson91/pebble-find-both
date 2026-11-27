# Find Both — for Pebble Time Steel

Two-way finder for a Pebble Time Steel (basalt, 144×168) paired to an iPhone.
Built with Pebble SDK 4.44, in C + PebbleKit JS. No speaker on the Time Steel,
so the watch side is vibrate + backlight flash only.

## The two apps

| App | File | What it does |
| --- | --- | --- |
| **Find Phone** | `find-phone.pbw` | On the watch: open it, press SELECT, your iPhone rings — even on silent. Press SELECT again to stop. Closing the app stops it too. |
| **Find Watch** | `find-watch.pbw` | Hidden app — it does not appear in the watch launcher. Launch it from the Pebble app on your phone and the watch buzzes and flashes for 5 minutes (timer on screen). Any button on the watch stops it. |

Both only work in Bluetooth range. If the phone is out of range, the watch
can't reach it — that's a Bluetooth limit, not an app bug.

## Install (iPhone)

1. Get both `.pbw` files onto your iPhone (AirDrop, email, Files).
2. Tap a `.pbw`, Share → open in the **Pebble** app. It installs to the watch.
   Do the same for the second file.
3. In the Pebble app, allow the **Find My Phone** permission when asked
   (Find Phone needs it to ring the phone). If ringing doesn't work, check
   Pebble app → Find Phone → permissions.
4. Tip: in the Pebble app, set **Quick Launch** to open Find Phone on a
   long-press, so finding your phone is one hold from the watchface.

## Use

- **Lost phone:** open Find Phone on the watch → SELECT → phone rings.
  SELECT again, or leave the app, to stop.
- **Lost watch:** open the Pebble app on your phone → your apps → tap
  **Find Watch**. The watch starts buzzing/flashing at once. Press any
  button on the watch when you find it.

## Build from source

The `.pbw` files are not committed to this repo (they're binaries). Every
push to `main` builds them in GitHub Actions — download them from the
Actions tab → the latest run → Artifacts → `pebble-find-both-pbw`.
Or build locally:

```sh
uv tool install pebble-tool --python 3.12
pebble sdk install latest
cd find-phone && pebble build   # writes build/find-phone.pbw
cd ../find-watch && pebble build
```

Notes: the SDK toolchain ships without the execute bit on some installs —
if `pebble build` says `arm-none-eabi-gcc is not executable`, run
`chmod -R a+x ~/.local/share/pebble-sdk/SDKs/4.44/toolchain/`.
Tested in the basalt emulator: both apps install cleanly.

## Credits

- Find Phone's ring mechanism is the Core Devices official pattern from
  `github.com/coredevices/pebble-apps/find-my-phone` — it subscribes to the
  Pebble app's built-in Find My Phone source, which is what makes the
  iPhone ring through silent mode.
- Find Watch is adapted from Find Watch 2 by Christian Ratzenhofer (MIT),
  `github.com/merlin1991/pebble-findwatch2`, simplified to a fixed
  5-minute vibrate + flash with no config page.
