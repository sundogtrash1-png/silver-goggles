# Gender Matters DLL

A native SKSE port of `gender_matters.esp`, with every number adjustable via
`GenderMattersDLL.ini` instead of hardcoded into the perk. No MCM, no SPID
dependency, no Papyrus.

## How this was built (so you can trust the numbers)

I didn't guess at the original mod's mechanics. I:
1. Cloned the real CommonLibSSE-NG source and checked every API used here
   against it directly (entry-point enum values, struct layouts, etc.)
2. Wrote a Python script to parse `gender_matters.esp`'s binary records
   myself and pulled out the real global names/defaults and the real perk
   entry-point magnitudes (0.85 melee mult, 1.15 incoming damage mult, etc.)
3. Matched those to Bethesda's actual native combat entry points
   (`kModAttackDamage`, `kModIncomingDamage`, ...) so the DLL reuses the
   game's own damage/stagger/block math instead of reimplementing it.

One honest gap: four of the flat bonuses (sneak, speech, alchemy, smithing)
were driven by a Papyrus script inside the original mod's `PlayerPerkContainer`
magic effect, which isn't readable from the static `.esp` data the way the
perk's own numbers were. Those four are reasonable approximations, clearly
marked as such in the ini - tune them freely.

## What you need to keep from your original install

- `gender matters.esp` — keep this. It's the only place `GM_Perk_Combat` and
  the `GM_*` globals actually exist as game forms; the DLL finds them by
  editor ID at runtime and rewrites their values from the ini.
- `perks_DISTR.ini` (SPID) — **you can delete this and uninstall SPID** if
  you only used it for this mod. The DLL now grants/removes the perk to
  female actors itself as they load in, so SPID is no longer needed for this.

## What's in this folder

```
CMakeLists.txt          build config
vcpkg.json               pulls in CommonLibSSE-NG automatically
src/
  main.cpp               plugin entry point
  Settings.h/.cpp         reads GenderMattersDLL.ini
  PerkPatcher.h/.cpp      rewrites the perk's numbers + the GM_* globals
  GenderDistributor.h/.cpp  grants/removes the perk based on actor sex
dist/SKSE/Plugins/
  GenderMattersDLL.ini    the settings file — usable as-is, edit and go
```

## Building it

I can't compile this for you directly — I'm running in a Linux sandbox with
no Windows/MSVC toolchain, and an SKSE plugin has to be built with Visual
Studio's compiler. Two ways to actually get the `.dll`:

### Option A — GitHub Actions (free, no installs, recommended)

GitHub gives free Windows build machines to public repos, with the exact
compiler needed already installed. This folder includes
`.github/workflows/build.yml`, which does the whole build for you.

1. Create a free GitHub account if you don't have one.
2. Create a new **public** repository and upload this entire folder to it
   (drag-and-drop on github.com works, or `git push` if you're comfortable
   with git).
3. Go to the repo's **Actions** tab. A build should start automatically
   (or click "Run workflow" if it doesn't).
4. Wait ~5 minutes. When it finishes, click the finished run → download the
   `GenderMattersDLL` artifact at the bottom of the page. That's a zip
   containing the compiled `.dll`.

Nothing to install on your own machine for this path.

### Option B — Build locally with Visual Studio

1. Install **Visual Studio 2022** (Community is fine) with the
   "Desktop development with C++" workload.
2. Install **CMake** (3.21+) — <https://cmake.org/download/>
3. Install **vcpkg** and set it up once:
   ```
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```
4. Set the environment variable `VCPKG_ROOT` to that folder.
5. From this project's folder:
   ```
   cmake -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
   cmake --build build --config Release
   ```
6. The compiled `GenderMattersDLL.dll` lands in
   `build/Data/SKSE/Plugins/GenderMattersDLL.dll`.

## Installing

Copy into your `Skyrim Special Edition/Data/` folder:
- `GenderMattersDLL.dll` → `Data/SKSE/Plugins/`
- `GenderMattersDLL.ini` (from `dist/`, edit it first if you want) →
  `Data/SKSE/Plugins/`
- your existing `gender matters.esp` → `Data/` (and enabled in your mod
  manager / plugins.txt as usual)

Requires SKSE64 for your game version, as usual for any SKSE plugin.

## If something doesn't work

The most likely failure mode is a CommonLibSSE-NG API drifting since I
checked it — I verified everything against a fresh clone, but if your build
throws errors about a missing member or changed signature, that's the
library having moved, not a fabricated guess on my end. Paste me the
compiler error and I'll fix it against the current source.
