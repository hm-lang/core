# Nullable

TODO: currently see [question mark operator](./question_mark.md), but maybe we should change
to using Q as a postfix on the type, e.g. intQ, so that it's more clear that it's the full
type.  E.g., `string? MaybeString` or `String? MaybeString` both make sense (if ? is a
MAYBE class under the hood), so switching to `stringQ MaybeString` will make it more clear.
