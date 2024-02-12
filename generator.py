 # Copyright (C) 2024  Adrien ARNAUD
 #
 # This program is free software: you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program. If not, see <https://www.gnu.org/licenses/>.

import os
import sys
import argparse

"""
 Simple .glsl to GLchar* converter for OpenGL glsl shader sources.
"""

def __parse_args():
  parser = argparse.ArgumentParser()
  parser.add_argument("--input", "-i", help="Input directory")
  parser.add_argument("--output", "-o", help="Output directory")

  args = parser.parse_args()
  if not args.input:
    print("Error, must specify an input path")
    exit()

  if not args.output:
    print("Error, must specify an output path")
    exit()

  return args

if __name__ == "__main__":
  args = __parse_args()
  filenames = os.listdir(args.input)
  lines = ""
  for filename in filenames:
    with open(args.input +  "/" + filename, "r") as fp:
      lines += "\nstatic const GLchar __{}[] =\n{{\n"\
      .format(os.path.splitext(filename)[0])
      for line in fp:
        lines += ("  \" " + line[:-1] + " " * (83 - len(line)) + " \\n\"\n")
    lines += ("};\n")

    res = """\
/*
 * Copyright (C) 2024  Adrien ARNAUD
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <glad/glad.h>
{}
  """ . format(lines)

  with open(args.output + "/shader_sources.h", "w") as fp:
    fp.write(res)
