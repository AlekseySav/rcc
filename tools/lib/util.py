from collections.abc import Iterator
import os
import subprocess
import tomllib
from typing import Any


def create(name: str) -> str:
	os.makedirs(os.path.dirname(name), exist_ok=True)
	return name


def filetree(root: str) -> Iterator[str]:
	for (path, _, files) in os.walk(root):
		for file in files:
			yield f'{path}/{file}'


def glob2regex(p: str) -> str:
	r = '\\.^$+{}|()'
	for c in r:
		p = p.replace(c, '\\' + c)
	p = p.replace('**', '.*').replace('*', '[^/]*').replace('?', '[^/]')
	return f'(^{p}$)'


def toml(path: str) -> dict[str, Any]:
	with open(path, 'rb') as f:
		return tomllib.load(f)


def run(*args, shell=False) -> str:
	p = subprocess.run(*args, shell=shell, stdout=subprocess.PIPE)
	p.check_returncode()
	return p.stdout.decode()
