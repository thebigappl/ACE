# ACE - Aaron's Chess Engine
ACE (Aaron's Chess Engine) is a chess engine written from scratch in C++20.

What started as a personal project out of my own love for chess has become a deep dive into chess programming, search algorithms, position evaluation, and performance optimisation.

The goal isn't just to build an engine that plays chess, but to understand how chess engines work under the hood and gradually develop ACE into a stronger and more capable opponent.

This is an ongoing personal project, and I'm excited to keep improving it as I learn more about chess programming.

## Features

- **C++20** - Built from scratch using modern C++.
- **UCI protocol support** - Compatible with chess GUIs that support the Universal Chess Interface.
- **10×12 mailbox board representation** - Efficient board representation for move generation.
- **Legal move generation** - Supports castling, en passant, and all four promotion choices.
- **Negamax search** - Recursive search framework with alpha-beta pruning.
- **Quiescence search** - Extends tactical lines beyond the normal search depth.
- **Iterative deepening** - Progressively searches to greater depths.
- **Move ordering** - Includes MVV-LVA capture ordering and promotion ordering.
- **Transposition table** - Stores previously searched positions using Zobrist hashing.
- **Incremental Zobrist hashing** - Efficiently updates position hashes after moves.
- **Make/unmake move system** - Applies and reverses moves during search.
- **Material-based evaluation** - Currently evaluates positions primarily by material balance.

## Building

### Requirements
- A C++20-compatible version of `g++`
- GNU Make

### Compile

Clone the repository:

```bash
git clone https://github.com/thebigappl/ACE.git
cd ACE
```

Build the engine:

```bash
make
```

This compiles the source files using the project's Makefile and produces an executable named `chess.exe`.

### Clean

To remove the compiled engine and generated test files:

```bash
make clean
```

## Running ACE

ACE communicates through the Universal Chess Interface (UCI) and can be used with compatible chess GUIs.

It has been tested with [Cute Chess](https://cutechess.com/).

To use ACE:

1. Build the engine using the instructions above.
2. Open your preferred UCI-compatible chess GUI.
3. Add ACE as a new engine.
4. Select the compiled `chess.exe` executable.
5. Configure the engine settings and start a game!

## Development Roadmap

ACE is an ongoing project, and there are still plenty of things I'd like to explore and improve.

Some of the next goals include:

- **Threefold repetition detection** - Recognise repeated positions and handle repetition draws.
- **Killer move heuristic** - Improve move ordering by prioritising quiet moves that have previously caused beta cutoffs.
- **History heuristic** - Use the search history to improve the ordering of quiet moves.
- **Static Exchange Evaluation (SEE)** - Improve capture ordering by estimating the outcome of exchanges on a square.
- **Improved position evaluation** - Move beyond material balance by exploring piece-square tables, pawn structure, king safety, and other positional factors.
- **Further search optimisations** - Experiment with techniques such as aspiration windows, Principal Variation Search, and Late Move Reductions.

## Why I Built ACE

I think the development of ACE began mostly out of me getting back into chess and accidentally becoming addicted to it all over again. It also stemmed from a deep ongoing desire to just create something no matter how bad it is. But instead of creating a To-Do or Weather app, this is what I've decided to work on.

For a bit of context, I've been taking a Data Structures & Algorithms (DSA) class at university which has made me deeply interested in bettering my understanding of complex algorithms while peeling away the high-level abstractions provided by languages such as Python. We've been doing a LOT of C. Of course, there's nothing wrong with C but I opted to use C++ for this project because I believed it to be a cooler C - like C's big brother.

Improving my C++ skills has become one of the most important focuses of this project for me, alongside the simple goal of just making cool stuff. I wanted something that would challenge me, improve my C++ and DSA skills, and push me beyond what I was already comfortable with. Building a chess engine seemed like the full package, and it was perfectly daunting given that I'd never done any chess programming before.

By adopting a positive mindset, I've learnt broadly to break down complex technical problems into smaller, more digestible subproblems. This has pushed me further and further every time I felt like giving up. It has also been incredibly satisfying to watch everything come together slowly: getting the first legal moves working, watching it search deeper, fixing bugs, and then finally playing against my own creation.

The progression from an engine that only made illegal moves at every turn, to only dishing out pawn pushes, to finally destroying me has felt like watching a chess apprentice rapidly improve until it eventually beats the sensei. An amazing feeling.

While I'm proud of the progress I've made so far, there's still a lot to do and plenty left to learn. One of the biggest things I want to tackle next is ACE's tendency to repeat moves when playing against itself, along with the head-scratching openings it sometimes decides to pull (h4 is a BAD starting move).

With that being said, I'm excited for what's ahead. I can confidently say that I'll keep working on ACE for as long as there are things left to learn and improve.