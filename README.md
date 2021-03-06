# chromium.bb

This repository contains the version of Chromium used by Bloomberg, along with
all the modifications made by Bloomberg in order to use it in our environment.

This repository serves a couple of purposes for us:

* **Provide a trimmed snapshot of the Chromium tree**
  A typical Chromium checkout is about 3GB and fetches code from several
different repositories.  Most of that code is not used by Bloomberg (we only
use the `content` portion of Chromium, and only for the Windows platform).

  Our checkout is about 300MB, and this all comes from a single repo, which
makes it much easier for us to use internally.

* **Provide a space for us to make/publish changes**
  We have made a bunch of changes to different parts of Chromium in order to
make it behave the way we need it to.  Some of these changes are bugfixes that
can be submitted upstream, and some of them are just changes that we
specifically wanted for our product, but may not be useful or desirable
upstream.  Each change is made on a separate branch.  This allows us, at
release time, to pick and choose which bugfixes/features we want to include in
the release.

  Note that while most of our bugfixes are not submitted upstream, it is our
intention to submit as many bugfixes upstream as we can.

  The list of branches along with descriptions and test cases can be found
[here](http://bloomberg.github.com/chromium.bb/).


## Branch Structure

The `master` branch contains the code that is used to make the official
Chromium builds used by Bloomberg.  It is not used for development.  Actual
development happens in one of the `bugfix/*` or `feature/*` branches.

Each `bugfix/*` or `feature/*` branch is based on the `upstream/latest` branch,
which contains a snapshot of the code we get from the upstream Chromium
project.  (Note: The `upstream/latest` is not an *exact* copy of the upstream
Chromium repo.  There are some minor modifications made to the build files and
code, in order to aid and optimize our tree minimization process).

The `release/candidate` branch contains changes that are scheduled to be
included in the next release.


## Build Instructions

**Bloomberg Employees:** [http://bburl/Bs67K](http://bburl/Bs67K)

If you are **not** a Bloomberg employee, the following instructions should still
work:

* Setup your build environment, as per [these
  instructions](http://www.chromium.org/developers/how-tos/build-instructions-windows).
  You'll need to follow the instructions for VS2010.
* Run the following command from inside the top-level 'chromium/' directory:

            gclient runhooks

* Open `chromium/src/blpwtk2/blpwtk2.sln`.  This solution file should be
  generated from the previous step.
* Build the `blpwtk2_all` project.
* Now, you can either run the `content_shell` project or the `blpwtk2_shell`
  project.
    * The `content_shell` project is from the upstream Chromium project, and
      uses the `content` layer directly.
    * The `blpwtk2_shell` project is from our `feature/blpwtk2` branch, and
      uses our `blpwtk2` integration layer.

---
###### Microsoft, Windows, Visual Studio and ClearType are registered trademarks of Microsoft Corp.
###### Firefox is a registered trademark of Mozilla Corp.
###### Chrome is a registered trademark of Google Inc.
