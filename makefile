
.PHONY: zip

ZIPNAME=mml2midfe_`date +%Y%m%d`
ARC = release/MML2MIDfe.exe mml2midfe.txt

zip:
	zip -9j $(ZIPNAME) $(ARC)
