# Asset Impact Analyzer

**Asset Impact Analyzer** is an Unreal Engine editor plugin that helps understand the **impact and risk of asset changes**.

It analyzes asset dependencies and referencers, builds a dependency graph, and estimates how widely a change to a selected asset may propagate through a project.

The goal is to make asset-level change risk **visible and measurable** during development.

---

## Why This Exists

In larger Unreal projects, changing a single asset can have unexpected side effects:
- materials affecting many meshes
- shared assets referenced across levels
- hidden transitive dependencies

Asset Impact Analyzer makes these relationships explicit so developers can reason about change impact before making edits.

---

## What It Does

- Builds **dependency and referencer graphs** for selected assets
- Distinguishes **direct** vs **transitive** impact
- Computes an **impact severity score** based on affected asset types
- Supports filtering (Engine, Plugins, World Partition internals)
- Exports detailed analysis results to **JSON**
- Runs entirely as an **editor-only**, non-intrusive plugin

---

## How It Works

1. Select one or more assets in the Content Browser
2. Run **Analyze Impact**
3. The plugin:
   - traverses dependency and referencer relationships
   - aggregates affected assets
   - computes an overall severity score
   - exports a structured report

Results are logged to the Output Log and saved as JSON files.

---

## Output

For each analyzed asset, the plugin generates a report containing:
- root asset
- overall severity and score
- impacted assets
- impact grouped by asset type

Reports are written to:
```
(Project)/Saved/Analysis/
```

---

## Example Content

Minimal example assets are included in:
```
Content/Examples/
```

Dependency chain:
```
Texture → Material → Skeletal Mesh → Level
```

These can be used to quickly verify:
- direct vs transitive impact
- severity scoring behavior

---

## Installation

1. Clone or download this repository
2. Place the plugin in:
   ```
   (Project)/Plugins/AssetImpactAnalyzer
   ```
3. Open the project in Unreal Engine
4. Enable **Asset Impact Analyzer** in:
   ```
   Edit → Plugins → Tools
   ```
5. Restart the editor

---

## Requirements

- Unreal Engine 5.x
- Editor build (plugin is editor-only)