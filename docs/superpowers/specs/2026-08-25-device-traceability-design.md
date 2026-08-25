# NiceHatThanks Device Inventory, Factory-Test and Order Traceability Design

Date: 25 August 2026
Status: Approved architecture, pending implementation plan

## Purpose

Create a small, dependable, local-first operations system for every physical Scoopy and populated PCBA that can answer:

- which Scoopy serial and ESP32 MAC identify the unit;
- which hardware type it is;
- which firmware files and versions have been programmed onto it;
- whether its latest production programming/test cycle left it safe to ship;
- which Stripe order and order-line slot it was assigned to;
- what firmware actually shipped;
- when it was dispatched;
- whether it was later returned, replaced, quarantined, repaired, used for development, or scrapped.

The system must prevent accidental allocation or dispatch of an invalid unit while staying understandable and repairable by one operator.

## Sources of truth

Responsibilities remain deliberately separated:

- **Stripe / NiceHatThanks order API** is the commercial truth for payment, order contents, quantities, customer details, delivery address, shipping, refunds, and commercial fulfilment state.
- **Local SQLite operations database** is the truth for physical devices, programming/test history, physical state, device/order assignment history, and the firmware actually associated with a shipment.
- **Excel finance workbook** is reporting/accounting data and may contain commercial/customer data already supplied by the order API.
- **CSV inventory export** is a human-readable report generated from SQLite. It is never read as an operational source after migration.

Customer name, email and postal address are not copied into SQLite. Stripe Checkout Session/order IDs are sufficient for physical traceability.

## Existing systems

### Flasher

`firmware/tools/flasher.py` currently performs the complete production cycle:

1. detect ESP32-C3;
2. read MAC;
3. derive serial from the last six MAC hexadecimal digits;
4. ask Presence versus Compact;
5. erase and flash factory-test firmware;
6. run automated hardware/button/Wi-Fi testing;
7. require mmWave Presence pass for Presence hardware;
8. require operator LED confirmation;
9. erase factory-test firmware;
10. flash the latest production firmware;
11. verify chip and MAC;
12. update CSV inventory only after the complete successful cycle.

The flashing/test sequence is proven and should not be restructured merely to introduce SQLite. The inventory persistence layer and failure-history recording are the intended changes.

### Fulfilment application

The PySide6 application reads paid live orders from `/api/admin/orders`, shows products and quantities, keeps printable-part progress locally, syncs an Excel workbook, and calls `/api/admin/dispatch` to send the dispatch email and mark Stripe fulfilment metadata as dispatched.

The desktop application will become the user interface for physical-unit assignment and dispatch validation. The Cloudflare Worker will remain commercially focused and will not become a device-inventory backend.

## File locations

### Canonical writable database

Windows default:

```text
%LOCALAPPDATA%\NiceHatThanks\operations\nicehatthanks.db
```

This is the only writable master copy.

For tests/development, the path may be overridden by an environment variable so automated tests never touch production data:

```text
NICEHATTHANKS_DB_PATH
```

### Google Drive operations area

Configured production default:

```text
G:\My Drive\NiceHatThanks\Operations\
```

Subdirectories already created:

```text
inventory\
finance\
backups\
```

Recommended optional override:

```text
NICEHATTHANKS_OPERATIONS_DIR
```

Google Drive contains exports, finance reporting and completed database snapshots only. The live SQLite database is never opened from Google Drive.

### Generated outputs

```text
G:\My Drive\NiceHatThanks\Operations\inventory\deviceInventory.csv
G:\My Drive\NiceHatThanks\Operations\finance\nicehatthanks-orders-live.xlsx
G:\My Drive\NiceHatThanks\Operations\backups\nicehatthanks-<UTC timestamp>.db
```

A missing Google Drive mount does not invalidate a successful local SQLite transaction. It produces a prominent export/backup warning and can be retried later.

## SQLite connection policy

Every process opens its own short-lived or scoped SQLite connection to the same local file.

Each connection enables:

```text
PRAGMA foreign_keys = ON
PRAGMA journal_mode = WAL
PRAGMA synchronous = FULL
PRAGMA busy_timeout = 5000
```

State-changing operations use short transactions, normally `BEGIN IMMEDIATE`, to serialize competing writers before validating and updating shared state. Hardware flashing, network calls and operator interaction must never hold an SQLite transaction open.

Schema versioning uses `PRAGMA user_version` rather than introducing a separate metadata table for the initial system.

## Schema

The minimum operational schema consists of four history-bearing tables:

- `devices`
- `flash_events`
- `assignments`
- `device_events`

No local customer or order-detail table is required.

### devices

One row per physical ESP32-based unit.

```text
devices
-------
device_serial             TEXT PRIMARY KEY
mac                       TEXT NOT NULL UNIQUE
device_type               TEXT NOT NULL
chip                      TEXT NOT NULL
first_seen_at             TEXT NOT NULL

current_firmware_version  TEXT
current_firmware_file     TEXT
last_flashed_at           TEXT

factory_test_status       TEXT NOT NULL
factory_tested_at         TEXT

status                    TEXT NOT NULL
status_reason             TEXT
notes                     TEXT NOT NULL DEFAULT ''

legacy_flash_count        INTEGER NOT NULL DEFAULT 0

created_at                TEXT NOT NULL
updated_at                TEXT NOT NULL
```

Constraints:

```text
device_type IN ('presence', 'compact')
factory_test_status IN ('not_tested', 'pass', 'legacy_pass', 'fail')
status IN ('available', 'quarantined', 'development', 'returned', 'scrapped')
legacy_flash_count >= 0
```

`status` describes the unit's physical/operational disposition, not whether it is assigned or sold. Assignment/dispatched state is derived from `assignments`.

`status_reason` distinguishes automatic and manual states, for example:

```text
available      / NULL
quarantined    / factory_test_failed
quarantined    / production_flash_failed
quarantined    / manual
quarantined    / legacy_sold_unlinked
development    / bench_unit
returned       / awaiting_inspection
scrapped       / damaged
```

A successful programming cycle may automatically clear only an automatic programming/test quarantine. It must not silently clear manual quarantine, development, returned or scrapped state.

### flash_events

One row per production programming attempt, including failures and interrupted attempts.

```text
flash_events
------------
id                        INTEGER PRIMARY KEY

device_serial             TEXT NOT NULL
started_at                TEXT NOT NULL
completed_at              TEXT

source                    TEXT NOT NULL
stage                     TEXT NOT NULL

device_type               TEXT NOT NULL
mac                       TEXT NOT NULL
chip                      TEXT

factory_test_firmware     TEXT
firmware_version          TEXT
firmware_file             TEXT

factory_test_result       TEXT NOT NULL
production_flash_result   TEXT NOT NULL

failure_stage             TEXT
failure_reason            TEXT
operator_override         INTEGER NOT NULL DEFAULT 0

FOREIGN KEY(device_serial) REFERENCES devices(device_serial)
```

Suggested result values:

```text
factory_test_result:
    not_completed
    pass
    legacy_pass
    fail

production_flash_result:
    not_attempted
    pass
    fail
    unknown_legacy
```

`source` initially supports `flasher` and `legacy_csv_import`.

`stage` records the last known milestone for diagnostics and crash history, for example `created`, `factory_erase`, `factory_flash`, `factory_test`, `led_confirm`, `production_erase`, `production_flash`, `verify`, `complete`, `failed`, `interrupted`.

A flash event is created and committed immediately before the first destructive erase. The connection is then released. Milestones/final results are updated in separate short transactions.

Failed attempts are retained permanently. They are manufacturing/repair history and are particularly useful because the same development units are intentionally reflashed many times while validating YAML/firmware changes.

### assignments

One row per physical device-to-order-slot assignment. Rows are never deleted by normal application workflows.

```text
assignments
-----------
id                        INTEGER PRIMARY KEY

device_serial             TEXT NOT NULL
order_id                  TEXT NOT NULL
order_line                INTEGER NOT NULL
unit_number               INTEGER NOT NULL

product_key               TEXT NOT NULL
required_device_type      TEXT NOT NULL

assigned_at               TEXT NOT NULL
released_at               TEXT
dispatch_prepared_at      TEXT
dispatched_at             TEXT
returned_at               TEXT

status                    TEXT NOT NULL

shipped_firmware_version  TEXT
shipped_firmware_file     TEXT

replaces_assignment_id    INTEGER
notes                     TEXT NOT NULL DEFAULT ''

FOREIGN KEY(device_serial) REFERENCES devices(device_serial)
FOREIGN KEY(replaces_assignment_id) REFERENCES assignments(id)
```

Assignment status values:

```text
assigned
released
dispatched
returned
```

A replacement creates a new assignment row with `replaces_assignment_id` pointing to the earlier assignment. The earlier assignment remains dispatched if the original unit has not physically returned; it changes to `returned` only when the physical unit is actually received back.

This distinction allows an original dispatched device and a later replacement device to coexist historically for the same order slot.

### device_events

Lifecycle history not naturally represented by a flash event or assignment.

```text
device_events
-------------
id                        INTEGER PRIMARY KEY
device_serial             TEXT NOT NULL
event_type                TEXT NOT NULL
timestamp                 TEXT NOT NULL
related_assignment_id     INTEGER
notes                     TEXT NOT NULL DEFAULT ''

FOREIGN KEY(device_serial) REFERENCES devices(device_serial)
FOREIGN KEY(related_assignment_id) REFERENCES assignments(id)
```

Initial event types may include:

```text
quarantined
released_from_quarantine
marked_development
returned
repaired
scrapped
sold_device_reflash_override
legacy_sold_import
note
```

The vocabulary can grow without schema changes.

## Database uniqueness and integrity rules

### Device identity

SQLite enforces both:

```text
PRIMARY KEY(device_serial)
UNIQUE(mac)
```

The flasher derives `scoopy-XXXXXX` from the final six MAC hexadecimal digits, but a six-digit suffix collision remains theoretically possible. Before any erase, the flasher checks both directions:

- same MAC under a different serial -> stop as database inconsistency;
- generated serial already belonging to a different MAC -> stop as serial collision.

No automatic identity rewrite is allowed.

### One physical device cannot serve two current orders

```sql
CREATE UNIQUE INDEX one_current_assignment_per_device
ON assignments(device_serial)
WHERE status IN ('assigned', 'dispatched');
```

A returned/released unit ceases to be current and may later be assigned again if its physical state permits.

### One pending assignment per order slot

Historical replacements mean multiple dispatched assignments may legitimately exist for the same logical order slot over time. The database therefore only prohibits two simultaneously pending assignments for one slot:

```sql
CREATE UNIQUE INDEX one_pending_assignment_per_order_slot
ON assignments(order_id, order_line, unit_number)
WHERE status = 'assigned';
```

Replacement assignment uses the same logical slot and points to the previous assignment with `replaces_assignment_id`.

## Product-to-device compatibility

Stable product keys from the order API map as follows:

```text
scoopy          -> presence
pcba_mmwave     -> presence
scoopy_compact  -> compact
pcba            -> compact
```

The fulfilment UI constructs one physical slot per quantity. Example:

```text
Scoopy x2
Scoopy Compact x1
```

becomes:

```text
line 1 / unit 1 -> presence
line 1 / unit 2 -> presence
line 2 / unit 1 -> compact
```

Each slot receives a distinct assignment row and physical serial.

## Derived device availability

Friendly states such as `ASSIGNED` and `DISPATCHED` are computed; they are not duplicated into `devices.status`.

A device is allocatable only when all of the following are true:

- `devices.status = 'available'`;
- `factory_test_status IN ('pass', 'legacy_pass')`;
- current production firmware version/file are present;
- the latest programming attempt does not leave the device in a failed/incomplete destructive state;
- there is no assignment with status `assigned` or `dispatched`;
- required device type matches the order slot.

The normal picker shows only allocatable, type-compatible devices. A secondary diagnostic view may explain excluded devices as `NOT TESTED`, `LATEST TEST FAILED`, `WRONG TYPE`, `ALREADY ASSIGNED`, `DISPATCHED`, `QUARANTINED`, `DEVELOPMENT`, `RETURNED`, or `SCRAPPED`.

## Flasher integration

### Preflight

After reading MAC and deriving serial, but before erasing flash:

1. open/validate the database;
2. detect serial/MAC collisions;
3. look up the current device and assignment history;
4. if a dispatched assignment exists, show a prominent sold-device warning and require explicit operator confirmation to continue;
5. ask/select hardware type;
6. identify intended current production firmware;
7. create a new device row if first seen, initially non-allocatable;
8. create a `flash_events` attempt row;
9. commit and close the write transaction;
10. begin destructive flashing.

If SQLite is unavailable, corrupt or cannot accept the attempt record, production flashing stops before erase.

### First-seen device

A previously unknown unit is inserted before destructive work so a failed first factory test is still traceable. Initial state is non-allocatable, for example:

```text
factory_test_status = not_tested
status = quarantined
status_reason = not_tested
```

A complete success changes it to normal stock unless the operator has deliberately designated a non-stock state.

### Successful complete cycle

After factory test, LED confirmation, production flash and final MAC/chip verification all succeed:

- complete the `flash_events` row with both test and production results `pass`;
- update `devices.current_firmware_version` and `current_firmware_file`;
- set `last_flashed_at`;
- set `factory_test_status = pass` and `factory_tested_at` to the latest explicit successful factory test time;
- clear only automatic `not_tested`, `factory_test_failed` or `production_flash_failed` quarantine back to `available`;
- preserve manual quarantine/development/returned/scrapped state;
- keep any existing assignment untouched;
- regenerate the Drive CSV export;
- request a database snapshot if backup policy says one is due.

### Failed or interrupted cycle

A failed attempt is finalized in `flash_events` with its exact stage/reason where available.

State reflects what is actually known:

- failure before factory PASS -> `factory_test_status = fail`, automatic quarantine;
- factory test PASS but later production flash/verification failure -> factory test remains known PASS, but current production firmware is not trusted and the unit is automatically quarantined with a production failure reason;
- once destructive erase has begun, failed/incomplete programming must not leave stale current production firmware values presented as trustworthy;
- a later complete PASS may clear an automatic programming/test quarantine;
- historical failed events remain forever.

Thus repeated test-board history may be `PASS, PASS, FAIL, FAIL, PASS`; the final PASS makes the unit good again unless another deliberate physical state blocks allocation.

### Assigned but not dispatched reflash

Allowed.

The assignment remains `assigned`. The new flash event/current firmware is recorded. If successful, the order now points to the same serial with the newly current firmware. If it fails, the assignment remains reserved but dispatch is blocked until the device passes again or the operator releases/replaces the assignment.

### Dispatched-device reflash

The flasher displays the order ID and dispatch history and requires explicit confirmation. Continuing sets `operator_override = 1` on the event and writes a `sold_device_reflash_override` device event.

The historical dispatched assignment is never removed or rewritten by flashing.

## Preserving the firmware that actually shipped

`devices.current_firmware_*` describes the device now and may change after returns or later servicing.

At dispatch preparation the assignment snapshots:

```text
shipped_firmware_version
shipped_firmware_file
```

These values are historical shipment evidence and are not updated by later reflashes.

## Fulfilment assignment workflow

For every order item that maps to physical hardware, the GUI displays one selector per physical quantity.

Example:

```text
Devices

Scoopy
Unit 1 of 2
[ Select device ]

scoopy-21771c
Presence
Firmware v0.1.3
Factory test: PASS
Last programmed: 25 Aug 2026
AVAILABLE
```

Assignment action runs inside `BEGIN IMMEDIATE` and revalidates eligibility in SQL before inserting. The UI does not trust an old dropdown result if another process changed the database.

Unassign before dispatch changes the row to:

```text
status = released
released_at = <UTC time>
```

Normal workflows do not delete assignment rows.

## Dispatch validation and cross-system consistency

SQLite and Stripe cannot share one ACID transaction. Dispatch therefore uses a prepare -> remote -> finalize protocol.

### 1. Local prepare

Inside a short `BEGIN IMMEDIATE` transaction, re-read the latest order slots and validate every required physical assignment:

- every required slot has exactly one assigned unit;
- no serial fills two slots;
- device type matches;
- device remains physically allocatable except for being assigned to this order;
- explicit/legacy factory test requirement is satisfied;
- current production firmware is present/trusted;
- device is not quarantined/development/returned/scrapped;
- assignment belongs to this order and slot.

Then snapshot current firmware onto each assignment and set `dispatch_prepared_at`. Commit before making any network request.

### 2. Remote commercial dispatch

Call existing `/api/admin/dispatch`.

The Worker remains responsible for final commercial checks, email idempotency, and Stripe fulfilment metadata.

The remote endpoint must additionally reject a fully refunded order. Checkout Session `payment_status = paid` is not treated as sufficient evidence that no refund has occurred; the Worker should inspect the associated PaymentIntent/latest Charge refund amount during dispatch validation. Partial refunds should be surfaced to the operator rather than automatically interpreted as a cancelled whole order.

### 3. Local finalize

On successful dispatch, including an idempotent `alreadyDispatched` response, update all prepared assignments for that order:

```text
status = dispatched
dispatched_at = Stripe/remote dispatched timestamp where available
```

The shipped firmware snapshot remains immutable.

### Failure/recovery cases

- **API fails normally:** clear stale preparation fields/snapshots or re-prepare on the next attempt; assignments remain `assigned`.
- **App crashes after local prepare but before Stripe dispatch:** next refresh sees Stripe pending and clears/re-prepares the stale local preparation.
- **Stripe succeeds but app crashes before local finalize:** next refresh sees Stripe dispatched plus locally prepared assignments and safely finalizes them without sending a duplicate email.
- **Stripe says dispatched but local assignments are missing:** show a critical reconciliation warning. Never invent serials after the fact.
- **Local says dispatched but Stripe says pending:** show a critical inconsistency warning; do not silently reverse local history.

The primary dispatch button is disabled before confirmation whenever any physical order slot is missing an assignment or a local blocker is known. The transaction-time preflight remains authoritative.

## Refunds and cancellations

Stripe remains the commercial authority.

### Before dispatch

A fully refunded/cancelled order must not be dispatchable. Existing assigned rows are preserved until the operator releases them, individually or with `Release all assignments`.

Release does not erase history and returns an otherwise-good device to allocatable stock.

### After dispatch

A refund alone does not mean the physical unit has returned. Do not alter device or assignment state automatically merely because money was refunded.

A physical return is recorded separately when the hardware is actually received.

## Returns, repair and replacement

### Return

When a dispatched unit is physically received back:

```text
assignment.status = returned
assignment.returned_at = now
devices.status = returned
devices.status_reason = awaiting_inspection
```

Record a `returned` device event.

The unit is not allocatable until explicitly restored to stock after inspection/repair.

### Repair/retest

Normal flasher history records repair-era programming attempts. A successful complete flash/test does not automatically clear `returned`; the operator explicitly marks the inspected device available and may add a `repaired` event.

### Replacement

Create a new assignment for the same logical order slot using a different device and set:

```text
replaces_assignment_id = <original assignment id>
```

The original assignment remains historically dispatched unless/until the original physical unit is returned. The replacement progresses from `assigned` to `dispatched` as a new assignment history row.

Primary order dispatch email/Stripe metadata are already idempotent after the original shipment, so replacement shipment handling should be a distinct local action rather than pretending the original order is being dispatched for the first time again.

## Development, quarantine and scrap

A minimal inventory-management dialog in the desktop tool should provide explicit lifecycle actions rather than requiring manual SQL edits:

- mark/unmark development;
- quarantine with a note/reason;
- release manual quarantine back to stock only after confirmation;
- mark returned unit available after inspection;
- scrap a unit;
- show programming and assignment history.

These actions create `device_events` and update `devices.status` in one transaction.

`scrapped` is terminal in ordinary UI workflows; recovery requires an explicit exceptional action, not a normal stock button.

## CSV export

After migration, `deviceInventory.csv` is generated from SQLite and is never read back into operational state.

The export should remain convenient for manual inspection and may include derived fields such as:

```text
device_serial
device_type
mac
first_seen
last_flashed
chip
firmware_version
firmware_file
flash_count
factory_test_status
factory_tested_at
device_status
assignment_status
current_order_id
last_dispatched_at
notes
```

`sold`, `sold_at` and stored `flash_count` are not required as authoritative device columns. The CSV may still expose equivalent derived reporting values if useful.

Total flash count is calculated as:

```text
devices.legacy_flash_count + COUNT(flash_events)
```

CSV generation writes to a temporary file and atomically replaces the final export where possible so a failure does not truncate the previous good report.

## Migration of the existing CSV

The supplied production CSV on 25 August 2026 contains:

- 9 devices;
- 9 unique MAC addresses;
- 9 unique serials;
- all serials correctly derived from their MACs;
- 1 Presence device and 8 Compact devices;
- 39 total historical flashes;
- 1 row with explicit `factory_test_passed = yes` and exact factory-test timestamp;
- 8 legacy rows with blank factory-test fields;
- no sold rows;
- no invalid timestamps, types or flash counts.

### Migration safety

Before writing the canonical database:

1. validate required CSV headers;
2. validate every serial/MAC/type/timestamp/count;
3. reject duplicate MAC or serial;
4. archive the untouched source CSV into the Drive `inventory` folder with a timestamped `deviceInventory-original-...csv` filename;
5. create and populate a temporary local SQLite database;
6. run `PRAGMA integrity_check` or `quick_check`;
7. atomically rename the completed database into the canonical path;
8. generate a fresh SQLite-derived CSV export;
9. create the first Drive database snapshot.

Any error before final rename leaves the original CSV untouched and no partially migrated master database in service.

### Per-row migration

For a row with `flash_count = N`, create one `flash_events` row representing the known final CSV state and store:

```text
legacy_flash_count = max(N - 1, 0)
```

This preserves the exact historical count without fabricating event details that were never recorded.

For the current supplied CSV this yields 9 known imported final-state flash events plus 30 legacy unexpanded flashes, totalling the original 39.

Factory-test conversion:

```text
factory_test_passed = yes
    -> devices.factory_test_status = pass
    -> preserve factory_tested_at exactly

legacy blank field
    -> devices.factory_test_status = legacy_pass
    -> factory_tested_at = NULL
```

A legacy PASS is allocatable because the historical flasher workflow wrote inventory only after the full factory-test/production cycle, but it remains visibly distinguishable from explicitly timestamped modern PASS evidence.

Do not invent a test timestamp from `last_flashed`.

Preserve `first_seen`, `last_flashed`, chip, firmware details and notes exactly.

If a future/other legacy CSV contains `sold = yes` but no order ID, do not fabricate an assignment. Preserve the fact with a `legacy_sold_import` device event, use `sold_at` as its timestamp when valid, and quarantine the device as `legacy_sold_unlinked` until manually reconciled.

## Backup strategy

Backups are SQLite-safe snapshots, not file copies of an actively changing WAL database.

Use Python SQLite's backup API to create a completed standalone snapshot, verify it, then place it in:

```text
G:\My Drive\NiceHatThanks\Operations\backups\
```

Suggested naming:

```text
nicehatthanks-2026-08-25T231503Z.db
nicehatthanks-2026-08-25T231503Z.db.sha256
```

Initial policy prioritizes simplicity over pruning:

- create/verify a snapshot on application startup when no recent snapshot exists;
- request a snapshot after significant successful state changes (successful production flash, assignment/release, dispatch finalization, return/replacement, manual lifecycle change), rate-limited so repeated writes do not create excessive snapshots;
- retain timestamped snapshots initially rather than implementing automatic deletion in v1;
- add retention pruning later only if storage becomes meaningful.

Backup failure does not roll back a valid local transaction. The application reports the age of the last successful backup and warns when Drive is unavailable.

Restore is an explicit offline operation: close flasher/fulfilment applications, preserve the suspect current DB, verify the chosen snapshot, copy it to a temporary local path, run integrity check, then atomically replace the canonical DB. Never restore while either application is connected.

## Database-unavailable behaviour

If the canonical SQLite database is missing unexpectedly after migration, corrupt, locked beyond busy timeout, or otherwise unavailable:

- flasher stops before destructive erase/programming;
- fulfilment disables assignment/lifecycle/dispatch operations;
- existing remote Stripe order browsing may remain read-only;
- no fallback writable CSV database is created.

This prevents two masters from silently diverging.

## Google Drive-unavailable behaviour

Drive is secondary. If `G:` or the Operations directory is unavailable:

- local DB reads/writes continue;
- flashing/assignment/dispatch may continue if SQLite is healthy;
- CSV export, finance sync and backup report a warning/pending state;
- later successful operations retry export/backup.

## Finance workbook

Move the workbook output to:

```text
G:\My Drive\NiceHatThanks\Operations\finance\nicehatthanks-orders-live.xlsx
```

Keep current behaviour where an Excel file lock (`PermissionError`) does not block order retrieval or device fulfilment. Display `Ledger not synced - close the workbook in Excel and refresh` and preserve the existing workbook unchanged.

Finance is not involved in SQLite device transactions.

## UI design

The normal theme becomes predominantly:

```text
background: #F5F0E6
text/structure: #1C211B
```

Remove white card/panel backgrounds. Inputs may use subtle cream variants/borders, but white should no longer define the main panel/card hierarchy.

Reserve accent colours for meaningful state: pending, dispatched, printed, pass, warning/failure.

### Selected-order device section

Add a `Devices` section between the order-item table and fulfilment controls.

For every physical order slot show:

- product/slot (`Unit 1 of 2`);
- device selector filtered to compatible allocatable devices;
- selected serial;
- Presence/Compact type;
- firmware version;
- factory-test state;
- last programmed time;
- friendly current state;
- Release action before dispatch.

If no device can be selected, offer diagnostics rather than mixing invalid devices into the normal dropdown.

### Dispatch button

`Mark as dispatched and email customer` is disabled when any required slot is missing or a currently selected device has a known blocker. Transaction-time validation still runs after the operator clicks, because UI state may be stale.

### Inventory management

Use a small secondary dialog/window rather than crowding order details. It can list devices with status and expose lifecycle/history actions needed for development, quarantine, returns and scrap.

## Failure modes explicitly covered

- multiple devices per order -> unit-number assignment rows;
- multiple product types -> product-key/type mapping per slot;
- release before dispatch -> assignment becomes `released`, history retained;
- cancelled/fully refunded before dispatch -> remote gate blocks dispatch; operator releases assignments;
- partial refund -> warning/operator review rather than automatic physical-state change;
- refund after dispatch -> no automatic return assumption;
- replacement -> new linked assignment row;
- physical return -> assignment `returned`, device `returned` until inspection;
- repair -> normal flash history plus explicit lifecycle event/return-to-stock action;
- reflash assigned unit -> allowed, assignment retained;
- reflash dispatched unit -> explicit warning and audited override;
- failed factory test -> flash event retained; non-allocatable until later success;
- production flash failure after factory PASS -> production firmware untrusted; quarantined;
- development unit -> explicit non-allocatable physical state not cleared by test PASS;
- manual quarantine -> explicit non-allocatable state not automatically cleared;
- scrap -> terminal ordinary state;
- duplicate MAC -> database constraint/preflight stop;
- serial suffix collision -> preflight stop before erase;
- database unavailable -> no flashing/assignment/dispatch writes;
- Google Drive unavailable -> local DB continues, reporting/backup warns;
- Excel workbook open -> finance sync skipped with warning;
- backup restore -> explicit offline verified restore;
- accidental assignment removal -> no delete operation; release history retained;
- firmware release changes after order -> assigned unit may be safely reflashed; dispatch snapshots final firmware;
- firmware changes after shipment -> assignment snapshot preserves what actually shipped;
- app crash between local/remote dispatch stages -> prepare/reconcile protocol;
- simultaneous flasher/fulfilment access -> WAL, busy timeout, foreign keys and short `BEGIN IMMEDIATE` write transactions.

## Implementation boundaries

The initial implementation should be incremental and preserve rollback points:

1. shared SQLite/data-path module and schema;
2. one-time CSV migration + export + snapshot support;
3. flasher persistence integration and failure history;
4. fulfilment read-only device visibility;
5. assignment/release transactions and UI;
6. dispatch preflight + prepare/finalize reconciliation;
7. refund-aware remote dispatch validation;
8. lifecycle/inventory-management dialog;
9. return/replacement flow;
10. finance path/theme cleanup and final integration testing.

Operational `firmware/tools` files remain intentionally outside Git. Shared code used by those tools must be placed so the local flasher can import it without forcing private operational data into the repository.

No cloud database, ERP, customer table or background service is part of this design.

## Acceptance criteria

The system is complete when:

- a newly flashed physical unit is represented once by unique serial/MAC and has immutable programming history;
- repeated test flashes preserve failures but latest complete PASS restores eligibility when no deliberate blocker exists;
- existing CSV devices migrate without lost serials, counts, timestamps, firmware details or notes;
- old factory-tested rows become clearly identified `legacy_pass` rather than receiving fabricated timestamps;
- one physical unit cannot be actively allocated to two orders;
- all hardware quantities in an order require distinct compatible serial assignments;
- dispatch cannot proceed with missing/invalid assignments or a fully refunded order;
- the exact firmware associated with each shipment remains recoverable after later reflashes;
- releases, returns and replacements preserve history rather than overwriting it;
- flasher and fulfilment can operate concurrently without manual lock files;
- Google Drive loss does not corrupt or replace the canonical database;
- the DB can be restored from a verified timestamped Drive snapshot;
- CSV and Excel remain reports, never competing writable masters.
