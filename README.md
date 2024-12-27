# 351-redirect

# Moving torwards web server

## Things to Fix
* Multiple messages before dying (DONE)
* Not Die -- keep accepting multiple connections (DONE)
* Port change from command line (DONE)
* Multithreaded (DONE)
* If bind fails -> error message (DONE)

## Things to add/do
* Different Protocols () -> HTTP instead of "echo" (DONE)
* Implement the various routes (/static) (TRY)
* Create html to return (inital/debug) -> "return content" (DONE)
* be graceful (errors, or at the end of connecion) (DONE)

* Implement /static, /calc, /stats (directory access, math, and history of calls)