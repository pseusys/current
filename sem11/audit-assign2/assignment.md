# ISOM4300 Assignment 2

> by Aleksandr Sergeev, student number: `21158862`.

## Question 1

### Actions against ISACA and related threats

NB! Even though it would be possible to dive into details and find several ISACA recommendations violations in one particular part of the story, I decided to focus on different cases instead.  
Framework violations and threats related:

1. **Skipping IT due diligence exercise**  
    The general practice adviced by ISACA framework is conducting due diligence exercise before the deal is signed, not after.
    Skipping this exercise might result in paying too much for a potentially insecure or problematic asset.
    Moreover, in case the item being purchased violates any legal policies or regulations, resolving those violations might result in both financial and reputational risks.

2. **R-iPro solution data storage**  
    In R-iPro online store system solution, not only the data was stored in one copy only (despite a monthly remote data back service was offered by them), but also the database was not encrypted and the only account being used to access it had admin privileges.
    Multiple threats can arise from this fact, including physical or logical database corruption, information theft, unauthorised access, potential programming flaws leading to different enterprises using online storage system information conflicts, etc.

3. **SycGo unknown hosting location**  
    Not knowing the physical location properties of the SyncGo solution might also be problematic.
    Lack of the data localization information may lead to legal issues (in case even the country and jurisdiction is unknown), fault recovery difficulties if the system physical location is damaged and also access control problems (since it is not clear who exactly has access to the physical system location site).

### Risks based on threats described above

1. First and foremost, there might be a risk of financial losses after a damaged, insecure or vulnerable asset is bought.
    Instead of receiving profits from it, HJ Food Inc. might have to fix lots of issues in it first.
    Secondly, if there are any legal issues are found the new owner might be legally prosecuted.
    Similarly, if something happens to the system right after the the deal is concluded the new owner will be responsible for all the reputational issues of JPLine.

2. Any natural disasters that affect the database can paralyse the whole system, since the database is the single point of failure.
    Any successful cyberattack on the database server can grant access to all the unencrypted data.
    Moreover, if one of the online store system client is malicious or just irresponsible enough, they can corrupt the whole database using the single administrator account.
    As a result, all the online store system data can be either lost, corrupted or stolen.

3. Once again, keeping user information in an unknown location may first of all result in legal risks.
    Especially since MIS is located in Germany, the user data keeping regulations in EU and in HK differ significantly - and there will be no way to verify compliance to the local rules.
    Not being able to access the system location might also be problematic if for instance there will be a need to check the system audit log, finally that might result in some malicious action being unnoticed.
    Finally, even though the speed of transaction processing was specifically noticed by the users, having all the data stored remotely in an unknwn location might eventually result in networking delays, especially if this location is far away (e.g. in Germany) and is not duplicated.

### Adviced countermeasures

1. Ideally performing a due diligence exercise before the deal conclusion is advisable.
    However, in case it is not an acceptable way, different other agreements might be considered.
    E.g., HJ Food Inc. could pay some price in advance (enough for Moe to proceed with her new business ideas) and after an independant audit is concluded and the true JPLine value is established. HJ Food Inc. would pay the rest.
    Finally, if HJ Food Inc. is totally sure JPLine value (client database, food storage and delivery system, etc.) is greater than the value they are about to pay no matter what security risks would be identified during due diligence exercise, this issue can be neglected (but in that case, however, HJ Food Inc. senior management should be prepared to reimplement all the JPLine IT system from scratch, what they actually eventually did).

2. Keeping all the data in an encrypted database, on a dedicated server, having a separate user account with no administrator permissions for every enterprise, so that each online store system operates independantly and does not interfere with the other instances.
    It would also be useful to keep a backup copy of the whole database off-site and update it regularly.
    In addition, it would be useful to perform audit from time to time to identify risks, adjust specific controls and ensure the solution complies to local regulations (because judging by the system description, it seems like it has never been audited since 2008).

3. In my opinion, the best solution would be moving the whole system into cloud.
    On one hand, that would transfer hosting responsibilities to a trusted cloud provider, on the other hand it would ensure there are no network delays and the real physical code location is as close to the end users as possible.
    Even though the exact location of cloud datacentres is also often unknown, at least location jurisdiction is clear and it is easier to decide what legal regulations exactly need to be followed.
    Moreover, many cloud providers offer automatic backup services and also allow easy access to audit traces if needed.

## Question 2

### A few risks related to the new SycGo system development

1. **Scope creep**  
    Even though the contract that HJ Food Inc. has signed with HPIS includes regular maintenance, it is important to stick to required changes only and not to expand the new system uncontrollably.
    Not only it can become slower and buggy in that case, but also introducing many new features can be costly, since only first 50 hours of work are free (according to the contract).

2. **Integration issues**  
    It is important to ensure that the new system is well-integrated with the offline business process of JPLine, including food stores, operators and delivery.
    If the system is not easy to use, is vulnerable to the same risks as the previous one (e.g. synchronization issues) or is incompatible with the existing user database scheme, it might disrupt business process in the future.

3. **Employees training**  
    Similarly to the previous risk, all the JPLine employees should be sufficiently trained to use the new updated system.
    Lack of their expertise might result in productivity decrease, human mistakes and in the end customer dissatisfaction.

4. **Vendor lock**  
    It would be useful to ask MIS to provide technical documentation for their product in addition to its support and afterwards hand in the documented source code to an escrow agency.
    That way HJ Food Inc. would ensure that they are not much dependant on their code vendor and the SycGo system can be maintained by another IT company even if MIS goes bancrupt.

5. **Data storage**  
    HJ Food Inc. senior management should definitely ask MIS developers to either move the SycGo servers to public cloud or at least give direct access to them to the management itself and also their internal audit team.
    It would help ensuring all the data is securely stored and backed up, storage complies to the local legal regulations and audit logs are available for regular checkup.

### SycGo system application-level controls

Designing input and output controls for the new SycGo system, I would learn from the mistaks of the previous online store system.

#### Input controls

1. **Limit check**  
    First of all, once transactions in the new system are made atomic, it is important to ensure strict limit checks for all the data received from the clients.
    Clients should never be able to order more goods than there are available in stock, because not being able to process an order is one of the most basic and common reputational risks for an online store.
    No customer will be satisfied if their order can not be delivered.

2. **Sequence check**  
    All the orders should be numbered consecutively and stored in a database, so that all the incoming and outgoing good numbers can be calculated and verified.
    Additional control action can be performed periodically in order to make sure that all the transactions are recorded properly, for example checksums of all the sold goods can be calculated and compared to the checksums of the goods remained in stock.

3. **Completeness check**  
    Even though there were no completeness-related issues described in the online store system, this check is very useful for all kind of online stores.
    Ensuring that user has correctly inputted all the information required for order processing is crucial for fast and efficient order preparation and delivery workflow.

#### Output controls

1. **Automatic updates**  
    Automatically updating in-stock good numbers on the store website would improve user experience, ensuring users would never be misled by ordering goods that have already been reserved for someone else.
    The best decision would be to partially reload store website every time a related order is made to ensure there is no time delay between the good number change in stock and on user screen.

2. **Permission reduction**  
    Fine-grained permissions for database management could help different types of users (including clients, JPLine employees, courriers, etc.) to interact with data storage without fearing to corrupt or break anything.
    Moreover, that would prevent even unintentional access to any classified data by the users that do not have sufficient level of permissions.

### Online store system changeover techniques

Since the online store system is critical for JPLine, smooth parallel changeover would be most advisable.
Some of the physical stores can be operating in pilot mode with the new system (if the physical stores interact with online store at all, e.g. for good in storage accounting, delivery ordering, etc.), while the others still use the old one.
In case physical facilities do not use online store system at all, a transition similar to A/B testing can be applied, i.e. users are divided in two random groups, one of them using the old online store system and the other one the new one.  
However, if for some reason parallel can not be implemented or is too expensive to maintain, abrupt changeover might also be considered.
The new online store system has been outsorsed and sufficiently tested, the contract also includes some free manhours for maintenance, so the probability and cost of SycGo system failure can be astemated as relatively low.
However, since the system is still critical, it would be adviced to perform abrupt changeover during nighttime (around 8pm UTC) for it not to be noticed by the majority of existing users.

### Changeover risks and controls

The obvious risks that can appear during changeover are connected to possible bugs in the new SycGo system.
These may include system crashes preventing users from placing orders, database corruption and data loss incidents, security issues allowing users to get inadequate rights, graphical user interface bugs negatively affecting user experience and so on.
For any type of changeover obvious detective control would be implementing both automated and manual incident reporting systems.
Automated reporting system would send crash, error and warning reports to the system developers as soon as they occur, manual reporting would include prompting users to leave feedback after their interaction with the new experimental SycGo system interface.  
As for the detective and corrective controls, they would vary depending on selected changeover type:

1. **Parallel changeover**  
    In a way, parallel changeover itself already includes a corrective control: if there are any bugs detected in the new system, pilot sites can be rolled back to using old system until the bugs are fixed.
    Automating this rollback would be the preventive control I suggest, as soon as the new system crashes or encounters an unrecoverable error, the old system is automatically being loaded.
    That would prevent negative user experience, all the users will still be able to use the online store even if the SycGo system is completely broken.
    A notification popup window with HJ Food Inc. IT team excuses for the new system failure can even further mitigate negative user experience.

2. **Abrupt changeover**  
    As for the abrupt changeover, the adviced preventive control would be creating a 24/7 phone hotline for order processing and prompting all the users that have encountered an error with the SycGo system to use the hotline to place their orders until the online store system issues are resolved.
    Of course, not being able to place an order online would make users upset, however at least they still will not remain hungry and probably can understand that JPLine is having technical issues only because it is trying to increase the online store quality.
    Speaking about corrective contorl, I would suggest keeping the old system snapshot and database image during changeover for it to be possible to undo the changeover if something goes critically wrong and apply emergency fixes.

### Post-implementation review

The review should be performed 3 to 6 months after the changeover.
It should be conducted by the HJ Food Inc. internal audit team, reporting to the senior management.
They should review the new system controls (if they work as intended) and error logs.
They should also check change requests (if any) and review the MIS support team progress on the change implementation.
Last but not least, the general system performance, objectives and cost benefits should be studied and carefully reported.  
All these steps are important in order to identify potential post-implementation issues and evaluate implementation process at whole.
Also, MIS employees should not be invited for the review because otherwise there would be a conflict of interests.
