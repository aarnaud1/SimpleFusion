# Copyright (C) 2024 Adrien ARNAUD
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
import math
import argparse

pi = 3.14159265358979323846

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', '-o', required=True,
                        type=str, help='Output dir')
    parser.add_argument('-n', type=int, default=1, help='Number of points')
    parser.add_argument('-d', type=float, default=1.0,
                        help='Distance from the camera to the center')
    return parser.parse_args()

if __name__ == '__main__':
    args = parse_args()

    outDir = args.output
    n = args.n
    d = args.d
    step = pi / float(n)
    
    fp = open(os.path.join(outDir, 'trajectory.txt'), 'w')
    for i in range(n):
        alpha = float(i) * step - pi / 2.0
        tz = -d
        ty = -1.0
        tx = 0.0
        qx = 0.0
        qy = math.sin(alpha / 2)
        qz = 0.0
        qw = math.cos(alpha / 2)
        fp.write('{} {} {} {} {} {} {}\n'.format(tx, ty, tz, qx, qy, qz, qw))
    fp.close()
    
    
