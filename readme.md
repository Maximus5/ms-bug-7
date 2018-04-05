# AnimateWindow problem

AnimateWindow can't be used like nice animated version of window minimization.

The following call just hides the window, but keyboard focus remains in that hidden window.

```
AnimateWindow(hWnd, 2000, AW_SLIDE | AW_VER_NEGATIVE | AW_HIDE);
```
