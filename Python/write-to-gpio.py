fw = file("/sys/class/gpio/export", "w")
fw.write("%d" % (12))
fw.close()
