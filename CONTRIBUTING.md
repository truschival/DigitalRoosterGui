# Contributing

There are many ways to contribute to DigitalRooster for everyone at all skills
and skill levels - not just programmers. If you are not into coding but still
want to improve the system, file a bug or feature request, write documentation,
design a nicer UI or website or just tell your friends about the project.

Also check out the other projects of DigitalRooster:
-   [buildroot_digitalrooster](https://github.com/truschival/buildroot_digitalrooster)
    Buildroot configuration to cross compile DigitalRooster for the target
	hardware

-   [DigitalRooster.dev](https://github.com/truschival/DigitalRooster.dev)
    Source of the website

Here is a pretty good write-up on 
[contributing to open source.](https://opensource.guide/how-to-contribute/)

It is a good idea to open an issue in the respective project beforehand
to share the thought and reach consensus before doing the programming.

## Social rules

*TL;DR:* This project welcomes everybody who acts decently and professionally.
Communicate with the other as if you are sitting together at your grandparents
dining table. If your grandma would frown upon your statement it is not
appropriate for this project either.

## Some technical rules

-   This project uses
    [git flow](https://nvie.com/posts/a-successful-git-branching-model/)
    as development workflow. All features are developed in feature branches.
	Only trivial corrections are fixed on the branch 'develop' directly.

-   Create a pull request against the 'develop' branch.

-   Write meaningful git
    [commit messages](https://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)

-   You should sign-off your commits (``git commit -s``) using a PGP key. This
	`Signed-Off-by: your full name <email@address>` means that you publish your
	changes to the project under the projects license (GPLv3) and you are
	allowed to do this.

-   **Code Style** - easy just automate it use the .clang-format file on
    your code ``clang-format style=file``
	I prefer ``snake_case`` for variables and methods but use ``PascalCase``
	for classes - but this is no hard rule that prevents a merge.

-   **Naming** - give the variables, classes, file names etc. meaningful names.
    *Again, name it professionally without slur (remember you want to show it
    to your grandma)*

-   **Comments** Check your comments, when you are done! The comment should
    match the code, be concise and contain relevant information

-   **Write Tests** - code without unit test cases will most likely not get
    merged

Note: Not all changes get immediately (or eventually) merged, even if all rules
are met.
