# test the lexer

# __debug__ is a special symbol
print(type(__debug__))

# short input
exec("")
exec("\n")
exec("\n\n")
exec("\r")
exec("\r\r")
print(eval("1"))
print(eval("12"))
print(eval("123"))
print(eval("1\n"))
print(eval("12\n"))
print(eval("123\n"))
print(eval("1\r"))
print(eval("12\r"))
print(eval("123\r"))

# lots of indentation
def a(x):
 if x:
  if x:
   if x:
    if x:
     if x:
      if x:
       if x:
        if x:
         if x:
          if x:
           if x:
            if x:
             if x:
              if x:
               if x:
                print(x)
a(1)
