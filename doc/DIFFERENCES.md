# RACK VS CARDINAL DIFFERENCES

This document describes the differences between VCV Rack official plugin and Cardinal.  
It is not possible to know all the internal details of the official plugin due to it not being open-source,
so more technical details are best-guesses based on its behaviour.

The obvious big difference is that the official plugin is a commercial, closed-source product while Cardinal is free and open-source.

Also, the official plugin works pretty much like the free standalone where you login and download modules.  
This is intentionally not allowed/enabled in Cardinal, as the target is to make a self-contained plugin binary.
Online access is also not allowed.

Bellow follows a list of features comparing the official plugin to Cardinal.

| Feature                        | VCV Rack                  | Cardinal                        | Additional notes |
|--------------------------------|---------------------------|---------------------------------|------------------|
| Open-Source                    | No                        | Yes                             | |
| Free (of cost)                 | No                        | Yes                             | |
| Officially supported           | Yes, if you pay           | No, but you can fix it yourself | |
| Contains internal modules      | Core only                 | Everything is internal          | |
| Loads external modules         | Yes                       | No                              | |
| Supports commercial modules    | Yes                       | No                              | |
| Plugin in LV2 format           | No                        | Yes                             | |
| Plugin in VST2 format          | Yes                       | Yes                             | |
| Plugin in VST3 format          | No                        | WIP                             | |
| Multi-threaded engine          | Yes                       | No, uses host audio thread      | Intentional in Cardinal, for removing jitter |
| Supports ARM systems           | No                        | Yes                             | This means Apple M1 too yes |
| Synth plugin variant           | 16 ins, 16 outs           | 2 ins, 2 outs                   | |
| FX plugin variant              | 16 ins, 16 outs           | 2 ins, 2 outs                   | |
| Raw-CV plugin variant          | Unsupported               | 2 audio IO + 8 CV IO            | Available in JACK, LV2 and VST3 formats, not possible in VST2 |
| Arbitrary parameter automation | Yes                       | No                              | Unsupported in Cardinal, tricky to do for many plugin formats at once |
| Integrated plugin host         | No, Host payed separately | Yes, using Carla or Ildaeil     | Currently work-in-progress in Cardinal |
| v1 module compatibility        | No                        | No, but with less restrictions  | Modules can load resources at any point |
| Online phone-home              | Yes                       | No                              | Online access is strictly forbidden in Cardinal |
| Proper dark theme              | No, only room brightness  | Yes                             | CC-ND respected by leaving files intact, dark mode applied at runtime |
| Proper Linux headless mode     | No, always requires X11   | Yes                             | CC-ND respected by leaving files intact, dark mode applied at runtime |

Additionally, Cardinal contains the following built-in modules not present in the official plugin or standalone:

 * Aria Salvatrice modules (except Arcane related modules, due to their online requirement)
 * Carla Plugin Host
 * Ildaeil Host
 * Host Parameters (24 host-exposed parameters as CV sources)
 * Host Time (play, reset, bar, beat, tick, bar-phase and beat-phase CV sources)
 * Host CV (for the CV IO variant, allows direct CV access to/from the DAW)
