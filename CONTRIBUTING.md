# Contributing to 2 Ship 2 Harkinian

We are all humans volunteering our free time to make this port as good as it can be. These guidelines keep the project maintainable and keep contributions reviewable. If you have questions or concerns about any of it, let us know.

## Original game code

2Ship sits on top of the Majora's Mask [decomp project](https://github.com/zeldaret/mm), and we want the original game code (`mm/src`, `mm/include`) left as untouched as possible. Every edit there makes it harder to pull in upstream decomp changes, and harder to tell vanilla behavior apart from ours.

- **Put your code in `mm/2s2h/`.** Enhancements, fixes and new features live there and attach to the game through GameInteractor hooks.
- **At most, add a VB hook.** If the behavior you need to change is buried in original code, wrap the vanilla condition in `GameInteractor_Should(VB_..., <vanilla condition>, ...)` and register the override from `mm/2s2h/` with `COND_VB_SHOULD`. That should be the only change to the original file, and with no override registered the game behaves exactly as it did before. The codebase has tons of examples to follow, look around!

## Pull requests

- **Write the description yourself.** If you don't have the energy to write it, we won't have the energy to read it. AI generated text is only allowed inside a `<details>` block with a note disclosing such.
- **Keep it small.** One change per PR, limited in scope. Anything over a thousand changed lines will very likely be closed immediately.
- **Test it thoroughly.** This should be common sense, but you'd be surprised. I understand you may not be able to test Mac/Linux but they should at least be building successfully, if you run into issues here, just ask for help in the #development channel on Discord.
- **Match the codebase.** Follow the patterns already established in the code you are touching. Consistency is key.
- **Don't over-abstract.** If a block of code isn't used in three or more places, it probably doesn't need to be its own function. Focus on readability and maintainability over minimizing lines of code or being clever.
- **Comment sparingly.** A comment briefly explains why the code does something, or notes a side effect. Variable and function names should make the "what" clear on their own.
- **Stand behind it.** You are responsible for what you submit. Don't open a PR if you have no intention of fixing its bugs, iterating on feedback, or answering questions about it.

## AI

We are not anti-AI. We are anti-slop.

"AI" can mean everything from tab completion finishing a variable name for you, to asking a chatbot for advice, to prompting an entire feature into existence without reading a line of it. We can't realistically stop anyone from using it and we don't want to guess where on that range a PR sits, so here is what we expect:

- **Code only.** Absolutely no generative AI in non-code contributions. That includes textures, models, and audio such as music or sounds.
- **No fully AI-generated PRs.** Using AI for menial work is typically fine, handing us something an LLM produced start to finish is not.
- **Understand what you submit.** You should be able to explain every change and maintain it afterwards. Submitting code you don't understand shifts the work onto reviewers without adding value. Using AI to build your understanding is encouraged.
- **Disclose it.** Say whether AI was used and to what extent, so reviewers can calibrate their expectations. This is especially important for first time contributors, as we have zero insight on their prior experience working with software.
- **Bring something to the table.** If you fully generated your solution with a single prompt, why should we care? Any other drive by contributor or even one of us maintainers could have done the same thing.

### Slop

Slop is low-quality code that reduces the maintainability of the project. It will never be merged. Signs of it:

- Overly verbose or overly complex code
- Brittle or bug-prone code
- Superfluous changes
- Comments that explain what the code does line by line
- Comments or markdown in clearly AI-generated prose ("not x, not y, not z", piles of em-dashes)
