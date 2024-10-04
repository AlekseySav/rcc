import re
from typing import Dict, Iterator, List, Tuple
from dataclasses import dataclass, field

from . import util


MAKEFILE = '''\
CC = {cc}
CFLAGS = {cflags}

{name}: {objects}
\t@$(CC) $(CFLAGS) -o {name} {objects}
\t@echo linking... >&2

{objcode}
'''


@dataclass
class Config:
	name: str
	root: str
	sources: List[str]
	debug: bool = False
	cc: str = 'gcc'
	cflags: List[str] = field(default_factory=list)
	gen: Dict[str, str] = field(default_factory=dict)

	@property
	def flags(self):
		return self.cflags + (['-g', '-DDEBUG'] if self.debug else [])

	def rootpath(self, f: str) -> str:
		return f'{self.root}/{f}'

	def outpath(self, f: str) -> str:
		return util.create(f'{self.root}/.out/{self.name}/{f}')

	def sourcefiles(self, add_gen=False) -> Iterator[str]:
		r = '|'.join(map(lambda x: util.glob2regex(self.rootpath(x)), self.sources))
		if add_gen:
			r += '|' + util.glob2regex(self.outpath('src/*'))
		r = re.compile(r)
		for f in util.filetree(self.root):
			if r.match(f):
				yield f

	def build(self) -> str:
		for src, cmd in self.gen.items():
			util.run(cmd.format(env=self, output=self.outpath(f'src/{src}')), shell=True)
		self._gen_makefile()
		res = self.outpath(f'bin/{self.name}')
		util.run(['make', res, '-f', self.outpath('Makefile')])
		return res

	def _gen_makefile(self):
		sources = self.sourcefiles(add_gen=True)
		objects = {}
		for s in sources:
			obj, deps = self._objfile(s)
			objects[obj] = f'{obj}: {deps}\t@echo {s} >&2\n\t@$(CC) $(CFLAGS) -c -o {obj} {s}\n'
		with open(self.outpath('Makefile'), 'wt') as f:
			print(MAKEFILE.format(
				name=self.outpath(f'bin/{self.name}'),
				cc=self.cc,
				cflags=' '.join(self.flags),
				objects=' '.join(objects.keys()),
				objcode='\n'.join(objects.values()),
			), file=f)

	def _objfile(self, src: str) -> Tuple[str, str]:
		_, dep = util.run([self.cc, '-MM', src] + self.flags).split(':', maxsplit=1)
		return self.outpath(f'obj/{src}.o'), dep


def config(name, root='.') -> Config:
	path = name.split('.')
	spec = util.toml(f'{root}/make.toml')
	conf = spec['common']
	for p in path:
		spec = spec[p]
	return Config(name=name, root=root, **conf, **spec)
