.PHONY: clean

all: imgs

venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || virtualenv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	touch venv/touchfile

imgs: venv
	. venv/bin/activate; jupyter nbconvert --execute --to notebook --inplace notebook.ipynb
	rm -rf notebook.nbconvert.ipynb

clean:
	rm -rf venv
	rm -rf .ipynb_checkpoints
	rm -rf build/
	find -iname "*.pyc" -delete
