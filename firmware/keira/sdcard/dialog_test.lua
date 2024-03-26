test = dialog.keyboard("SAY MY NAME")

alert = alertDialog("HELLO", test)
alert.update()

util.sleep(3)

for i = 10, 1, -1 do
    alert.setTitle("GLORY TO")
    alert.setMessage("UKRAINE")
    alert.update()
    util.sleep(0.25)
    alert.setTitle("СМ**Ь")
    alert.setMessage("р**ні")
    alert.update()
    util.sleep(0.25)
end

progress = progressDialog("HELLO", test)

for i = 10, 1, -1 do
    alert.setProgress(i*10)
    alert.setMessage("РАКЕТА")
    alert.update()
    util.sleep(0.25)
    alert.setProgress(i*10+5)
    alert.setMessage("НА москву")
    alert.update()
    util.sleep(0.25)
end