#!/usr/bin/env python2.6
import unittest
from pyparsley import PyParsley
from inspect import currentframe
from os.path import dirname

class TestPyParsley(unittest.TestCase):
	
	def setUp(self):
		self.parsley = PyParsley({'title': 'title'})
		self.alt_parsley = PyParsley('{"title": "title"}')
		self.__file__ = currentframe().f_code.co_filename
		self.__dir__ = dirname(self.__file__)
		self.file = self.__dir__ + '/../../test/yelp.html'
		self.json = '{ "title": "\\t\\tNick\'s Crispy Tacos - Russian Hill - San Francisco, CA\\n" }'
	
	def test_file_xml(self):	
		parsed = self.parsley.parse(file = self.file, output = "json")
		self.assertEquals(self.json, parsed)
	
	def test_json_file_xml(self):	
		parsed = self.alt_parsley.parse(file = self.file, output = "json")
		self.assertEquals(self.json, parsed)
		
	def test_native(self):
		parsed = self.alt_parsley.parse(file = self.file, output = "python")
		self.assertEquals({ "title": "\t\tNick's Crispy Tacos - Russian Hill - San Francisco, CA\n" }, parsed)		

if __name__ == '__main__':
	unittest.main()