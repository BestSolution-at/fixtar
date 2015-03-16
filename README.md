ft ("fix tar")
=====

`ft` is a commandline tool to extract as much data from corrupted tar archives as possible.

Some background information about the tool can be found [here](http://riaschissl.bestsolution.at/2015/03/repair-corrupt-tar-archives-the-better-way/)
and in the docs folder of the project.


License
-------

`ft` is distributed under the terms of the [GNU General Public License (GPL), Version 2.0](http://www.gnu.org/licenses/gpl-2.0.html).
The text of the license is included in the file LICENSE in the root of the project.

The tool is &copy; by Thomas Graf, Thomas Graf (metaf4@users.askja.de)

Building
-------

In order to build ft, you will need the following build requisites:

- **gcc** (pretty much any version will do
- **flex**
- **sed**
- **coreutils**
- **make**

Once you have those tools, change into the `src` folder and invoke `make`.

`make test` will run a couple of tests and
`make install` will install the tool in /usr/local/bin

Installation
-------

After building, run `make install`. `ft` will then be installed in `/usr/local/bin`

Usage
-------
`ft` copies all data it can find, even if the data is moved within the archive.
Usage:
<blockquote>
<pre>
	ft &lt; damaged.tar &gt; repaired.tar
	ft &lt; damaged.tar | tee repaired.tar | tar -tf - | tee list.txt
	ft &lt; damaged.tar | tar --backup=numbered -xvf -
</pre>
</blockquote>

**Notes:**

- no options, no output.
- a cut off file is filled with a line break (an error message will be shown).
- it is not guaranteed that TAR extensions actually exist in the expected data.
In order to avoid unwanted overwriting, you should use `tar` as follows: `tar --backup=numbered -xvf repaired.tar`
or with option `"-k"`
- vendor/POSIX extensions often start with names like `./@LongLink`, `./@LongName` or `*/PaxHeader/*`,
those are not shown by `tar`, but `cpio` or `pax` do show them.
- it is quite difficult to deal with all possible situations, due to this the most likely have been chosen :)
