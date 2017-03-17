#!/usr/bin/python

import sys
import sqlite3
import re
from fuzzywuzzy import fuzz
from fuzzywuzzy import process

myName = str(sys.argv[1])

def ExtractConcept(fullName):
    allPhrases = {"left", "right", "medial", r'\bof\b', r'\bpart\b', r'\binterna\b', r'\(*\)', r'\band\b', r'\brh\b', r'\blh\b', r'[0-9]*'}
    concept = re.sub('_', " ", fullName)
    concept = re.sub('-', " ", concept)
    for phrase in allPhrases:
        concept = re.sub(phrase, "", concept)
    return concept

def RemoveSpatialTerms(fullName):
    allPhrases = {"left", "right", "medial"}
    concept = re.sub('_', " ", fullName)
    for phrase in allPhrases:
        concept = re.sub(phrase, "", concept)
    return concept

dbname = "/home/lorensen/ProjectsGIT/ta98-sqlite/db/ta98.sqlite"
con = sqlite3.connect(dbname)
cur = con.cursor()

cur.execute("select count(1) from ta98")

allConcepts = ExtractConcept(myName)

conceptList = re.sub("[^\w]", " ",  allConcepts).split()

# find all entries the

possibilities = set()
for concept in conceptList:
    concept_pattern = r'%{0}%'.format(concept)
    cmd = '''select ta98.name_en
            from synonyms join ta98 on synonyms.id = ta98.id
            where synonyms.synonym like ?'''
            
    for row in cur.execute(cmd, [concept_pattern]):
        possibilities.add(row[0])

# remove left/right from myName
nonspatialName = RemoveSpatialTerms(myName)

matches = process.extract(nonspatialName, possibilities, limit=5)
print myName + ": " + str(matches)
