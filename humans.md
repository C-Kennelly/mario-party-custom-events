# Working with Claude Sonnet 4 for Mario Party Event Development

This guide provides tips for human developers collaborating with Claude Sonnet 4 to create and modify Mario Party 3 custom events.

## Whenever you work with Claude Sonnet 4

### What I Handle Automatically
- **Workspace Awareness**: I automatically reference [agents.md](agents.md), existing templates, and your established patterns
- **Development Constraints**: I follow the self-contained file requirements, N64 SDK limitations, and PartyPlanner64 integration needs
- **Code Standards**: I maintain consistent formatting, naming conventions, and documentation styles from existing files
- **Fresh Perspective**: New chat sessions give you a clean slate while maintaining full workspace context

### What You Should Provide Explicitly  
- **Specific Requirements**: When you have particular mechanics, behaviors, or complexity levels in mind
- **Priority Constraints**: If certain aspects (ROM size, performance, Board RAM usage) are critical
- **Integration Needs**: How the event should interact with specific boards or other events
- **Testing Scenarios**: Edge cases or specific player situations you want handled

### Communication Tips
- **Be Direct**: Just say "create an event that does X" - I'll follow established patterns automatically
- **Provide Context**: Mention if this relates to existing events or builds on previous work  
- **Ask Questions**: I can explain implementation choices or suggest alternatives
- **Iterate Freely**: Don't hesitate to request changes or refinements

## When working with Claude Sonnet 4 to create a new event

### What I Handle Automatically
- **Workflow Following**: I'll use the development workflow from agents.md (template copying, header setup, etc.)
- **Directory Structure**: New events go to `In-Work/MP3/` with appropriate naming
- **Template Integration**: I'll start from `mp3_Event_Template.c` and customize appropriately  
- **Library Integration**: I'll identify needed `mp3lib.c` functions and plan for copy-paste integration
- **PartyPlanner64 Headers**: I'll set up proper NAME, GAMES, EXECUTION, and PARAM headers

### What You Should Specify
- **Event Concept**: Describe the core mechanic or player experience you want
- **Complexity Level**: Simple animation, interactive choice, or multi-turn persistent event
- **Player Impact**: How it affects coins, stars, positioning, or game state
- **Board RAM Needs**: Whether the event needs to persist data across turns
- **Parameter Requirements**: What options players configuring the event should have

### Examples of Helpful Requests
- ✅ "Create an event where players can spend coins to swap positions with another player"
- ✅ "Make a simple animation event that gives the current player 10 coins"
- ✅ "Design a multi-turn event that tracks which players visited it using Board RAM"
- ❌ "Make a cool event" (too vague)
- ❌ "Create something with networking" (not feasible in this environment)

## When working with Claude Sonnet 4 to modify an event in progress

### What I Handle Automatically
- **Code Analysis**: I'll read existing event files to understand current implementation
- **Pattern Preservation**: I'll maintain existing code style, variable naming, and structure
- **Dependency Tracking**: I'll identify which `mp3lib.c` functions are in use and preserve them
- **Context Integration**: I'll understand how changes fit with the existing event logic

### What You Should Provide Explicitly
- **Specific Files**: Point me to the exact files that need modification (e.g., "modify GameGuyLoyaltyProgram.c")
- **Change Description**: Describe what behavior should be different or what's not working
- **Scope Boundaries**: Whether to optimize, add features, fix bugs, or completely restructure
- **Testing Issues**: If you've encountered specific problems or edge cases

### Effective Modification Requests
- ✅ "In GameGuyLoyaltyProgram.c, change the coin reward from 10 to 15"
- ✅ "The SwapCharacter event isn't handling CPU players correctly - can you fix this?"
- ✅ "Add a parameter to TurnOrderTriggered.c to make the swap chance configurable"
- ✅ "Optimize the Board RAM usage in Ancient_Mews_Mystery.c to use fewer addresses"

### Debugging Collaboration
- **Share Error Messages**: Copy exact compiler errors or PartyPlanner64 warnings
- **Describe Unexpected Behavior**: What happens vs. what should happen
- **Testing Environment**: Mention if issues occur in specific emulators or boards
- **Previous Changes**: Reference what was modified recently if relevant

## Other

### Version Control Tips
- I can help update version numbers and changelog comments in event files
- When finalizing events, I'll remind you about the release checklist from agents.md
- I can suggest meaningful commit messages based on the changes made

### Performance Considerations  
- I'll automatically suggest ROM space optimizations (removing unused functions)
- Ask me to review Board RAM usage patterns if you're concerned about conflicts
- I can help identify opportunities to consolidate similar events

### Community Sharing
- I can format events for sharing on forums or repositories
- Ask me to generate documentation or usage examples for complex events
- I can help create board-specific event packages or themed collections

---

*This guide is a living document. Feel free to suggest improvements or additions based on your development experience.*