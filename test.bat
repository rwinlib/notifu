@echo off
rem ----------------------------------------------------------------------------
rem Notifu displays a message. So in it's simplest form, the message is the only 
rem required arguments. Use the /m switch
rem ----------------------------------------------------------------------------
release\notifu /m "This is a simple Notifu message."

rem ----------------------------------------------------------------------------
rem Most users will want their own prompt (or title also). Use the /p switch
rem ----------------------------------------------------------------------------
release\notifu /m "This is a simple Notifu message." /p "Simple prompt"

rem ----------------------------------------------------------------------------
rem You probably don't want it to stay up there forever. Use the /d switch (the 
rem delay is in milliseconds, with a 250ms resolution).
rem ----------------------------------------------------------------------------
release\notifu /m "This is a simple Notifu message with a 3 second delay." /d 3000

rem ----------------------------------------------------------------------------
rem If your message is important, you can change the icon Windows will use inside 
rem the balloon with the /t switch, followed by info, warn or error.
rem ----------------------------------------------------------------------------
release\notifu /m "This is a simple Notifu warning message." /t warn

rem ----------------------------------------------------------------------------
rem You might want to use a different icon in the system notification area. 
rem The /i switch tries it's best to extract an icon from the path you give it. 
rem It supports using environment variables and an icon number.
rem ----------------------------------------------------------------------------
release\notifu /m "This is a simple Notifu message with a custom icon." /i %SYSTEMROOT%\system32\shell32.dll,43

rem ----------------------------------------------------------------------------
rem Embeeded quotes are properly escaped
rem ----------------------------------------------------------------------------
release\notifu /m "\"Theo Est\" <test@sender.example.>" 

rem ----------------------------------------------------------------------------
rem Concatenation of multiple M and P parameters
rem ----------------------------------------------------------------------------
release\notifu /p "Version " /p 1.3 /m "This is " /m a /m " test of " /m concatenation


rem ----------------------------------------------------------------------------
rem Introducing carriage return and line feed for each /n encountered
rem ----------------------------------------------------------------------------
release\notifu /p "Version " /p 1.4 /m "This is " /m a /m " test of\n" /m returns /m "\n" /m "and other line feeds"
