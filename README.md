# Kodi fork

This is my personal Kodi fork.  Improvements compared to
[mainline Kodi](https://github.com/xbmc/xbmc):

- [Wayland](http://wayland.freedesktop.org/) support
  ([which was deleted from mainline](https://github.com/xbmc/xbmc/pull/8980#issuecomment-177113249))
- Mali GPU support, e.g. [Cubieboard](http://cubieboard.org/)
  ([rejected for no reason](https://github.com/xbmc/xbmc/pull/6268))
- out-of-tree build
  ([rejected because Kodi will reportedly migrate to cmake](https://github.com/xbmc/xbmc/pull/7059))
- lots of small code optimizations and bug fixes
  ([rejected for personal reasons](https://github.com/xbmc/xbmc/pull/8868))

[Max Kellermann](http://max.kellermann.name/)

## Insults?

Several Team Kodi members accused me of insulting
[Pär Björklund](https://github.com/Paxxi).  This is what Team Kodi member
[Keith Herrington](http://kodi.wiki/view/User:Keith) told me on IRC
(`cirrus` being my IRC nickname):

    14:02 <keith> yeah cirrus is just an asshole
    14:07 <keith> yeah because youre an asshole
    14:19 <keith> which you would have been rejected, since youre an asshole

Figuring out who insulted whom is left as an exercise to the reader.

([Full, unabbreviated IRC log](http://max.kellermann.name/download/kodi/irc/keith_asshole.txt))

## Banned

On March 4th 2016, Keith Herrington banned me from Kodi's IRC channel:

    21:47 <cirrus> BtbN: fix in my Kodi repository: https://github.com/MaxKellermann/xbmc/commit/95a573f6864c9a2e4c821cb618edd9854e7155ff
    21:47 <cirrus> Team Kodi is welcome to merge it.
    21:48 <BtbN> I'd guess just send a PR, should be a quick merge.
    21:50 <cirrus> no, I've stopped sending PRs after some Team Kodi members stated they won't accept any code from me.
    21:50 <cirrus> ask keith about the story :-)
    21:51 <cirrus> there are several crash bug fixes in my repository, which are not going to be merged.
    21:51 <keith> tldr; he's a difficult person to work with, continually said insulting things to other team members, does not play well with others
    21:51 <cirrus> insulting things?
    21:51 <keith> yes
    21:51 <cirrus> 14:02 <keith> yeah cirrus is just an asshole
    21:51 <cirrus> 14:07 <keith> yeah because youre an asshole
    21:51 <cirrus> 14:19 <keith> which you would have been rejected, since youre an asshole
    21:51 <cirrus> :-)
    21:51 <cirrus> like this?
    21:52 <keith> no, you said worse things than that, which is why i called you an asshole.
    21:52 <cirrus> what did I say?
    21:52 <keith> here, i'll just ban you, ok?
    21:52 <cirrus> what is worse than calling somebody "asshole"?
    21:52 -!- mode/#kodi [+o keith] by ChanServ
    21:52 <BtbN> ...
    21:52 <cirrus> hehe.
    21:52 <BtbN> fine, I'll PR it.
    21:52 -!- mode/#kodi [+b *!~max@2a01:4f8:d13:c02::dead:beef] by keith
    21:52 -!- cirrus was kicked from #kodi by keith [leave my name out your mouth]

## Why is Paxxi wrong?

Pär Björklund (aka Paxxi) is a distinguished member of the Kodi
developer community, but he made a few mistakes.  Everybody makes
mistakes, not a big deal.

    <cirrus> and he misunderstoof how reference counting works.
    <keith> i doubt that.
    <keith> paxxi's been doing c++ longer than you've probably been born

Discussion about these mistakes is impossible with people like Keith
Herrington, because if Paxxi said it, it must be right.

No, seriously, everybody makes mistakes.  Even somebody who has been
doing C++ already before C++ was invented.  (Fun fact for Keith: I
happen to be older than C++.)

### Reference Counting

[Original code review](https://github.com/xbmc/xbmc/pull/8868#discussion-diff-49933756).

    - CFileItemPtr fileItem = m_channelItems[iIndex];
    + const CFileItemPtr &fileItem = m_channelItems[iIndex];

Paxxi wrote:

> this breaks the ref counting [...] as there's no copy done the ref
> count in the shared_ptr isn't incremented, if another thread erases
> the vector the ref count might drop to 0 and the item is destroyed
> leaving a reference pointing into random memory

My reply:

> Looks like you don't understand how reference counting works. If
> what you describe were possible, we would be doomed already, because
> taking the reference would already crash.  If you already own the
> reference, you don't need to take another one - and if you don't own
> the reference, it's impossible to create another one.

The remaining discussion was pointless, because Paxxi didn't appear to
understand my argument.

Admittedly, the problem here is hard to understand.  It is easy to
believe that `std::shared_ptr` is some magic fairy dust which will
make your code thread-safe, but it's not!  A `std::shared_ptr`
instance **is not thread-safe**.  Only its control block is.

Evidence:

- http://en.cppreference.com/w/cpp/memory/shared_ptr
- http://www.boost.org/libs/smart_ptr/shared_ptr.htm#ThreadSafety
- https://gcc.gnu.org/onlinedocs/libstdc++/manual/memory.html
- https://msdn.microsoft.com/en-us/library/bb982026%28v=vs.110%29.aspx
- http://seanmiddleditch.com/dangers-of-stdshared_ptr/
- https://sigsegv.me/2013/12/02/shared_ptr-is-almost-thread-safe/

Bottom line (from these documents): you must protect access to
`std::shared_ptr` instances; either by guaranteeing that only one
thread will access it, or by serializing access (e.g. with a mutex).
(In C++11, you can also wrap std::shared_ptr in std::atomic, but
that's rather obscure, and still doesn't make it perfectly
thread-safe.)

Bottom line (for Kodi): Kodi is buggy, because it doesn't protect its
`std::shared_ptr` variables.  As long as people don't understand this
problem,
[my solution](https://github.com/MaxKellermann/xbmc/commit/93990e59ca41c8c4fb67443227ca37f4a14befa1)
will be rejected.

Right now, access to `std::shared_ptr` is unprotected, which is a
serious bug and crashes Kodi.  My patch changed the copy to a
reference; bug-wise, this doesn't make a difference: if it's illegal
to access the original `std::shared_ptr`, **taking a copy is illegal
as well**.  The commit in question doesn't change the nature of the
bug; it is just a micro-optimization.
[The mutex lock is really needed because it crashes Kodi](https://github.com/MaxKellermann/xbmc/commit/93990e59ca41c8c4fb67443227ca37f4a14befa1),
but this bug fix was rejected.

About reference counting: reference counting helps to share an object
instance with several clients.  If you "own" a reference (a
`std::shared_ptr` in C++), you can use it to create another reference,
and keep it or give it to somebody else.  If you don't "own" a
reference, you can't create another one (this is the part where Kodi
is buggy).

The first step to creating a new reference is ensuring that you have
one already; e.g. by locking a mutex that is documented to imply
reference ownership.  By unlocking the mutex, you lose ownership of
that reference.  If you took the chance to create another (private)
reference before unlocking, you can continue working with the shared
object until you give up that private reference.

I repeat: the mutex lock/unlock is missing in Kodi.  As long as the
code in question is within mutex protection, there is no reason to
create another reference (this is the part where Paxxi was wrong).
