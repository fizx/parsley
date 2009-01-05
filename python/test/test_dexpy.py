#!/usr/bin/env python
import unittest
from dexpy import DexPy
from inspect import currentframe
from os.path import dirname

class TestDexPy(unittest.TestCase):
	
	def setUp(self):
		self.dex = DexPy({'title': 'title'})
		self.alt_dex = DexPy('{"title": "title"}')
		self.__file__ = currentframe().f_code.co_filename
		self.__dir__ = dirname(self.__file__)
		self.file = self.__dir__ + '/../../test/yelp.html'
		self.json = '{ "title": "\\t\\tNick\'s Crispy Tacos - Russian Hill - San Francisco, CA\\n" }'
	
	def test_file_xml(self):	
		parsed = self.dex.parse(file = self.file, output = "json")
		self.assertEquals(self.json, parsed)
	
	def test_json_file_xml(self):	
		parsed = self.alt_dex.parse(file = self.file, output = "json")
		self.assertEquals(self.json, parsed)
		
	def test_native(self):
		parsed = self.alt_dex.parse(file = self.file, output = "python")
		self.assertEquals({ "title": "\t\tNick's Crispy Tacos - Russian Hill - San Francisco, CA\n" }, parsed)		

if __name__ == '__main__':
	unittest.main()