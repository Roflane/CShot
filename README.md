# CShot — screenshot utility

Captures screenshots via WinAPI and saves them as PNG.

**Stack:** C++, WinAPI, stb_image_write

### How it works
- Captures screen using GDI BitBlt
- Encodes and writes output via stb_image_write (header-only)
