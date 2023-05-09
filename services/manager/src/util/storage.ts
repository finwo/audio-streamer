import { observer } from '@finwo/observer';

type StorageValue = null | number | string | StorageEntry;
type StorageEntry = {
  [index:string]: StorageValue;
} | StorageValue[];

const cache: Record<string, any> = {};

export async function syncedObject<T extends StorageEntry>(name: string): Promise<T> {
  // Return from cache if possible, so it's the exact same object everywhere
  if (cache[name]) return cache[name];

  // Bind cache object to actual storage
  // @ts-ignore storage_get is set by webview wrapper
  const org: T = await storage_get(name) || {};
  const obs    = observer<T>(org, () => {
    // @ts-ignore storage_set is set by the webview wrapper
    storage_set(name, org);
  });

  // Return the newly constructed bound object
  return cache[name] = obs;
}
