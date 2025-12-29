
# Asset Impact Analyzer

**Asset Impact Analyzer** is an Unreal Engine editor plugin for analyzing asset dependency impact and change risk.

It builds a dependency + referencer graph for selected assets, computes an impact severity score, and exports detailed results for further inspection.

---

## Features

- Builds full **dependency and referencer graphs**
- Computes **impact severity** based on affected asset types
- Distinguishes **direct** and **transitive** impact
- Configurable filtering (Engine, Plugins, World Partition internals)
- Exports analysis results to **JSON**
- Includes **example assets** for quick testing
- Editor-only, non-intrusive plugin

## Installation

1. Clone or download this repository
2. Place the plugin in your project:

`(Project)/Plugins/AssetImpactAnalyzer`

3. Open the project in Unreal Engine
4. Enable **Asset Impact Analyzer** in:

`Edit → Plugins → Tools`

5. Restart the editor

## Usage

1. Open the **Content Browser**
2. Right-click one or more assets
3. Select **Analyze Impact**

The analysis will:
- Log a summary to the Output Log
- Export a JSON report to:

`(Project)/Saved/Analysis/`

Each selected asset generates a separate report.

## Example Content

The plugin ships with minimal example assets located in:

`Content/Example/`

Dependency chain:
`Texture → Material → Skeletal Mesh → Level`

Use these assets to quickly verify:
- direct impact
- transitive impact
- severity scoring

## Output Example
Impact analysis for: /Game/Example/Texture
Severity: HIGH (score 10)
Material: 1
Mesh: 1
Level: 1

JSON reports include:
- root asset
- severity and score
- impacted assets
- impact grouped by asset type

## Configuration

Analysis behavior is controlled internally via `FImpactAnalysisSettings`, including:

- Game-only filtering
- Engine / Plugin content inclusion
- World Partition internals exclusion
- Verbose output

The default settings are suitable for most projects.

## Requirements

- Unreal Engine 5.x
- Editor build (plugin is editor-only)
