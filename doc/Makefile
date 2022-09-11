.PHONY: clean

IMGS = img/f3D_ratio.png \
       img/fd.png \
       img/pot3D.png \
       img/vscrcl.png 

all: $(IMGS)

src/venv: src/venv/touchfile

src/venv/touchfile: src/requirements.txt
	test -d src/venv || python -m venv src/venv
	. src/venv/bin/activate; pip install -Ur src/requirements.txt
	touch src/venv/touchfile

img/%.png: src/make_%.py src/venv
	. src/venv/bin/activate; python $<

clean:
	rm -f $(IMGS)
	rm -rf src/venv
	rm -rf src/__pycache__/
	find src/ -name "*.pyc" -delete
