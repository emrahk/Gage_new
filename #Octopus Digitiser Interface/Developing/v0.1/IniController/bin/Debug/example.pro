PRO EXAMPLE

   Window, XSize=600, YSize=300
   !P.MULTI = [0, 3, 1]

      ; Draw the first plot. Keep info.

   PLOT, Findgen(11)
   p1 = !P & x1 = !X & y1 = !Y

      ; Draw the second plot. Keep info.

   PLOT, Findgen(11)
   p2 = !P & x2 = !X & y2 = !Y
      
      ; Draw the third plot. Keep info.

   PLOT, Findgen(11)
   p3 = !P & x3 = !X & y3 = !Y

      ;  Restore Plot2 info and overplot on the second plot.

   !P = p2 & !X = x2 & !Y = y2
   OPLOT, Reverse(Findgen(11))

END