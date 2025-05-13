# Testing Documentation

## Overview
The purpose of this document is to record decisions regarding the testing of the DungeonCrawl project.

[Test coverage]

## Decisions Made in Testing

### UI components
- **Description**: UI components with the sole purpose of displaying text for the player are playtested and don't have automated tests.
- **Reasoning**: Testing the UI through automation would only work if the components are fixed and won't change by using a framework or tool. Our UI components are subject to change so we deemed the effort not worthwhile at this stage.
- **Impact**: Visual bugs might appear in release version if they weren't found during playtesting.
