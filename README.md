# RandomSquareScreen

## Why

I've believed for some time there is an exploit with HDCP where the XOR stream
can be revealed by static content and through this revealing an adversary can
recover the key by collecting enough of the XOR stream.

## How

This program provides a window which updates it's drawable surface with random
squares. It does this on an interval. The window can be maximized on the
screen and then one only has to press any key and the window borders will
disappear leaving a window the exact size of the screen with randomly changing
pixels.

This should help hide all content on the screen that is static such as the
desktop, taskbar, and other things. 

After this is in place the user can use applications in the foreground over
the RandomSquareScreen. A second program WindowShake is then used to jiggle
these windows about randomly to prevent an adversary who has knowledge of
the applications open from using that information to reveal the XOR stream.
